# `HWCPipe 2.0`

## Introduction

Currently access to Hardware Counters is exposed via ioctl interfaces.
That requires some knowledge about driver internals and sometimes about hardware itself.
Every software that needs to access Mali counters needs to interface with internals somwehow.

HWCPipe 2.0 is a second version of currently existing open source solution maintained by Arm.
Its aim is to support up to date way to interface with the driver.
The library is intended to be more modular than its predecessor and support different use-cases.
