# brutebtc

a tool to check BTC address balances between a private key hex range

**WARNING:** it has only been tested on arch linux, and it may not work on other systems

## Main Command

```
Usage:
  brutebtc [command]

Available Commands:
  check       Check balances between hex range
  help        Help about any command

Flags:
  -h, --help      help for pascra
  -v, --version   version for pascra
```

## Check Command

```
Usage:
  brutebtc check [start hex] [end hex] [flags]

Flags:
  -d, --delay int       delay in milliseconds between each request (default 1000)
  -h, --help            help for fetch

```

## Contributing

pull requests are welcome to:

- revise the docs
- fix bugs or bad logic
- suggest/add features or improvements

## License

MIT
