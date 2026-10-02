# Security Policy

The web build of this firmware runs a Wi-Fi access point, a web server and an update endpoint inside a vehicle. Security reports are therefore taken seriously.

## Supported Versions

Security fixes are applied to the latest code on the default branch and to the most recent release.

## Reporting a Vulnerability

If you find a security issue, **please do not open a public issue**.

Instead, email **muksin.muksin04@gmail.com** with:

- A description of the issue and its potential impact
- Steps to reproduce (board, configuration, sketch)
- A suggested fix, if you have one

You will receive a response as soon as possible, and credit in the release notes if you wish.

## Notes for Users

- Change the default access point password (`AP_password` in `WebServer_Code_CAN/WEB_SERVER.ino`) before regular use.
- The web dashboard and the update endpoint have no authentication. Only connect the device to networks you trust.
- Unplug the device from the vehicle when it is not in use.
