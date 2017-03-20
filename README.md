<p align="center">
<img src="https://raw.githubusercontent.com/Morphux/Graphic/master/logo/single_penguin.png" /><br />
</p>
<p align="center">
<img src="https://img.shields.io/badge/language-c-blue.svg" /> &nbsp;
<h1 align="center" style="border:none">Morphux/Protocol</h1>
<h6 align="center">Morphux Package Manager Protocol Test Suite</h6>
</p>

**This repository contains tests, and only tests.**
**DO NOT run this code in production**

## Install
### Clone the repository
```
git clone --recursive https://github.com/Morphux/Protocol.git
```

### Compile
```
make
```

### Fake server
```
./protocol_test server <port> <IP>
```

### Fake client
```
./protocol_test -p <port> -a <IP> client
```
