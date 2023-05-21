package main

import (
	"log"

	"github.com/spf13/cobra"
	"github.com/z89/brutebtc/cmd"
)

var VERSION = "0.1.0"

type BruteBTC struct {
	RootCmd *cobra.Command
}

func (pascra *BruteBTC) Start() {
	pascra.RootCmd.AddCommand(cmd.Check())

	if err := pascra.RootCmd.Execute(); err != nil {
		log.Fatal(err)
	}
}

func New() *BruteBTC {
	return &BruteBTC{
		RootCmd: &cobra.Command{
			Use:     "brutebtc",
			Short:   "brutebtc - generate BTC addresses between private key hex values and display balances",
			Version: VERSION,

			CompletionOptions: cobra.CompletionOptions{
				DisableDefaultCmd: true,
			},
		},
	}
}

func main() {
	instance := New()
	instance.Start()
}
