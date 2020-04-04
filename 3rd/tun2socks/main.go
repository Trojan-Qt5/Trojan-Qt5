//reference https://github.com/trojan-gfw/igniter-go-libs/blob/master/tun2socks/tun2socks.go
package main

import (
	"C"
	
	"flag"
	"io"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"time"
	
	"github.com/eycorsican/go-tun2socks/common/log"
	_ "github.com/eycorsican/go-tun2socks/common/log/simple"
	"github.com/eycorsican/go-tun2socks/core"
	"github.com/eycorsican/go-tun2socks/tun"
)

var handlerCreater = make(map[string]func(), 0)

func registerHandlerCreater(name string, creater func()) {
	handlerCreater[name] = creater
}

type CmdArgs struct {
	ProxyServer     *string
	UdpTimeout      *time.Duration
}

type cmdFlag uint

const (
	fProxyServer cmdFlag = iota
	fUdpTimeout
)

var flagCreaters = map[cmdFlag]func(){
	fProxyServer: func() {
		if args.ProxyServer == nil {
			args.ProxyServer = flag.String("proxyServer", "1.2.3.4:1087", "Proxy server address")
		}
	},
	fUdpTimeout: func() {
		if args.UdpTimeout == nil {
			args.UdpTimeout = flag.Duration("udpTimeout", 1*time.Minute, "UDP session timeout")
		}
	},
}

func (a *CmdArgs) addFlag(f cmdFlag) {
	if fn, found := flagCreaters[f]; found && fn != nil {
		fn()
	} else {
		log.Fatalf("unsupported flag")
	}
}

var args = new(CmdArgs)

const (
	MTU = 1500
)

var (
	lwipWriter 			io.Writer
	tunDev              io.ReadWriteCloser
)

//export stop_tun2socks
func stop_tun2socks() {
	err := tunDev.Close()
	if err != nil {
		log.Fatalf("failed to close tun device: %v", err)
	}
}

//export run_tun2socks
func run_tun2socks(tunName *C.char, tunAddr *C.char, tunGw *C.char, tunDns *C.char, proxyServer *C.char) {

	// Reassign value of ProxyServer
	ProxyServer := C.GoString(proxyServer);
	args.ProxyServer = &ProxyServer;

	// Open the tun device.
	dnsServers := strings.Split(C.GoString(tunDns), ",")
	var err error
	tunDev, err = tun.OpenTunDevice(C.GoString(tunName), C.GoString(tunAddr), C.GoString(tunGw), "255.255.255.0", dnsServers, false)
	if err != nil {
		log.Fatalf("failed to open tun device: %v", err)
	}

	// Setup TCP/IP stack.
	lwipWriter := core.NewLWIPStack().(io.Writer)

	// Register TCP and UDP handlers to handle accepted connections.
	if creater, found := handlerCreater["socks"]; found {
		creater()
	} else {
		log.Fatalf("unsupported proxy type")
	}

	// Register an output callback to write packets output from lwip stack to tun
	// device, output function should be set before input any packets.
	core.RegisterOutputFn(func(data []byte) (int, error) {
		return tunDev.Write(data)
	})

	// Copy packets from tun device to lwip stack, it's the main loop.
	go func() {
		_, err := io.CopyBuffer(lwipWriter, tunDev, make([]byte, MTU))
		if err != nil {
			log.Fatalf("copying data failed: %v", err)
		}
	}()

	log.Infof("Running tun2socks")
	
	osSignals := make(chan os.Signal, 1)
	signal.Notify(osSignals, os.Interrupt, os.Kill, syscall.SIGTERM, syscall.SIGHUP)
	<-osSignals
}

func main() {
}