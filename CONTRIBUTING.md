# Contributing to OBD2 CAN Bus Reader

Thank you for taking the time to contribute! Every vehicle test, bug report, fix and idea makes this project better for the whole car-hacking and maker community.

By participating, you agree to follow the [Code of Conduct](CODE_OF_CONDUCT.md).

## Ways to Contribute

| | |
|---|---|
| 🚗 **Report a tested vehicle** | Tell us which cars work (or don't). Use the **Vehicle report** issue template. |
| 🐛 **Report a bug** | Use the **Bug report** template and include serial logs whenever possible. |
| 💡 **Suggest a feature** | Use the **Feature request** template. |
| 🔧 **Submit code** | New PIDs, board ports, protocol fixes, performance improvements. |
| 📝 **Improve the docs** | Typos, clearer instructions, wiring photos. |

## Reporting Bugs

Before opening an issue, please search the [existing issues](https://github.com/muki01/OBD2_CAN_Bus_Reader/issues). A good report includes:

- The build (`Basic_Code` or `WebServer_Code_CAN`) and the commit you are using
- The ESP32 board and the Arduino-ESP32 core version
- The CAN transceiver (TJA1050, SN65HVD230, …)
- The vehicle: make, model, year and engine
- The selected and the detected protocol
- **The serial debug output** (enable `DEBUG_Serial`). The frames sent and received are the most useful information.

## Development Workflow

1. **Fork** the repository and create a branch:
   ```bash
   git checkout -b feature/my-improvement
   ```
2. Make your changes, keeping them **focused**: one fix or feature per pull request.
3. **Test on real hardware** when your change touches the communication code, and say in the pull request which vehicle and protocol you tested on.
4. Make sure everything still **compiles** for the boards it supports.
5. Commit with a clear message.
6. Push and open a **pull request**, filling in the template.

## Coding Guidelines

- Follow the existing style of the file you are editing: naming, indentation and comment density.
- Wrap constant debug strings in `F()`.
- Keep the main loop **non-blocking**; the web server and the WebSocket share it with the CAN communication.
- Do not commit binaries, credentials or personal data (for example your own VIN) in code or logs.

## Web Dashboard Changes

The web interface source lives in **[OBD2 Diagnostic UI](https://github.com/muki01/OBD2-Diagnostic-UI)**. Please open pull requests for it there; `WebServer_Code_CAN/data` only holds the built files.

## License

By contributing, you agree that your contributions are licensed under the [GNU General Public License v3.0](LICENSE), and that the author may also offer them under a commercial license.
