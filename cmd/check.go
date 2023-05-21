package cmd

import (
	"encoding/hex"
	"fmt"
	"io"
	"log"
	"math/big"
	"net/http"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/btcsuite/btcd/btcec"
	"github.com/btcsuite/btcd/chaincfg"
	"github.com/btcsuite/btcutil"
	"github.com/spf13/cobra"
)

func hexToAddress(hexString string) (string, error) {
	// refer to this article for the conversion: https://www.freecodecamp.org/news/how-to-create-a-bitcoin-wallet-address-from-a-private-key-eca3ddd9c05f/

	// decode the hex value to bytes
	keyBytes, err := hex.DecodeString(hexString)
	if err != nil {
		return "", err
	}

	// convert the byte array to a private key
	_, pubKey := btcec.PrivKeyFromBytes(btcec.S256(), keyBytes)

	// convert the public key to a compressed format
	pubKeyBytes := pubKey.SerializeUncompressed()

	// generate the BTC address from the public key
	address, err := btcutil.NewAddressPubKey(pubKeyBytes, &chaincfg.MainNetParams)
	if err != nil {
		return "", err
	}

	return address.EncodeAddress(), nil
}

func hexTo64Characters(hexValue string) (string, error) {
	// ensure the hex value has is an even length
	if len(hexValue)%2 != 0 {
		hexValue = "0" + hexValue
	}

	// pad the hex value with leading zeros to make it 64 characters
	paddedHex := fmt.Sprintf("%064s", hexValue)

	return paddedHex, nil
}

func hexToDecimal(hexString string) (*big.Int, error) {
	// remove any "0x" prefix from the hex string, if present
	hexString = strings.TrimPrefix(hexString, "0x")

	// convert the hex string to a big Int
	big := new(big.Int)
	result, success := big.SetString(hexString, 16)
	if !success {
		log.Fatalf("invalid hex string")
		os.Exit(1)
	}

	return result, nil
}

func decimalToHex(decimal int64) (string, error) {
	hex := strconv.FormatInt(decimal, 16)

	return hex, nil
}

func Check() *cobra.Command {
	var delay int

	command := &cobra.Command{
		Use:   "check [start hex] [end hex]",
		Short: "Check balances between hex range",
		Long:  "Check base58 encoded BTC addresses from a private key hex range",
		Run: func(cmd *cobra.Command, args []string) {
			layout := "2006-01-02 03:04:05 PM AEST"

			fmt.Println("Started: ", time.Now().Format(layout))

			var startHexValue, _ = hexTo64Characters(args[0])
			var endHexValue, _ = hexTo64Characters(args[1])

			startValue, _ := hexToDecimal(startHexValue)
			endValue, _ := hexToDecimal(endHexValue)

			for i := startValue; i.Cmp(endValue) <= 0; i.Add(i, big.NewInt(1)) {
				// convert the current itterator to a padded hex value
				hex, _ := decimalToHex(i.Int64())
				paddedHex, _ := hexTo64Characters(hex)

				// convert the hex value to a BTC address
				address, err := hexToAddress(paddedHex)
				if err != nil {
					log.Fatal(err)
				}

				// create http client to call REST API
				var client http.Client

				// GET HTTP request
				resp, err := client.Get("https://blockchain.info/q/addressbalance/" + address)
				if err != nil {
					log.Fatal(err)
				}

				defer resp.Body.Close()

				// if the response status code is 200, read the response body into a byte array
				if resp.StatusCode == http.StatusOK {
					bodyBytes, err := io.ReadAll(resp.Body)
					if err != nil {
						log.Fatal(err)
					}

					// parse string to float64
					value, err := strconv.ParseFloat(string(bodyBytes), 64)

					if err != nil {
						fmt.Println("error parsing body for response")
						os.Exit(0)
					}

					// convert satoshi to BTC
					var bal = strconv.FormatFloat(value/100000000, 'f', -1, 64)

					// print result
					fmt.Printf("balance: %s BTC, address: %s\n", bal, address)
				} else {
					fmt.Println("failed to fetch balance for address: " + address)
				}

				// delay the http requests
				time.Sleep(time.Millisecond * time.Duration(delay))

			}

			fmt.Println("Finished: ", time.Now().Format(layout))
		},
	}

	command.Flags().IntVarP(
		&delay,
		"delay",
		"d",
		1000,
		"delay in milliseconds between each request",
	)

	return command
}
