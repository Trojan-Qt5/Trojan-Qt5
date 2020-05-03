//reference https://github.com/trojan-gfw/igniter-go-libs/blob/master/tun2socks/tun2socks.go
package main

import (
	"C"

	"context"
	"io"
	"io/ioutil"
	"net"
	"strings"
	"time"

	//"github.com/Trojan-Qt5/go-tun2socks/common/log"
	_ "github.com/Trojan-Qt5/go-tun2socks/common/log/simple"
	"github.com/Trojan-Qt5/go-tun2socks/core"
	"github.com/Trojan-Qt5/go-tun2socks/proxy/socks"
	"github.com/Trojan-Qt5/go-tun2socks/tun"

	_ "github.com/p4gefau1t/trojan-go/api"
	"github.com/p4gefau1t/trojan-go/common"
	"github.com/p4gefau1t/trojan-go/conf"
	"github.com/p4gefau1t/trojan-go/log"
	"github.com/p4gefau1t/trojan-go/proxy"
	_ "github.com/p4gefau1t/trojan-go/proxy"
	_ "github.com/p4gefau1t/trojan-go/proxy/client"
	_ "github.com/p4gefau1t/trojan-go/stat/memory"

	"github.com/Trojan-Qt5/go-shadowsocks2/cmd/shadowsocks"
)

const (
	MTU = 1500
)

var (
	client            common.Runnable
	lwipWriter        io.Writer
	tunDev            io.ReadWriteCloser
	ctx               context.Context
	cancel            context.CancelFunc
	isRunning         bool = false
	isTrojanGoRunning bool = false
)

//export is_tun2socks_running
func is_tun2socks_running() bool {
	return isRunning
}

//export stop_tun2socks
func stop_tun2socks() {
	log.Info("Stopping tun2socks")

	isRunning = false

	err := tunDev.Close()
	if err != nil {
		log.Fatalf("failed to close tun device: %v", err)
	}

	cancel()
}

//export run_tun2socks
func run_tun2socks(tunName *C.char, tunAddr *C.char, tunGw *C.char, tunDns *C.char, proxyServer *C.char) {

	// Open the tun device.
	dnsServers := strings.Split(C.GoString(tunDns), ",")
	var err error
	tunDev, err = tun.OpenTunDevice(C.GoString(tunName), C.GoString(tunAddr), C.GoString(tunGw), "255.255.255.0", dnsServers)
	if err != nil {
	}

	// Setup TCP/IP stack.
	lwipWriter := core.NewLWIPStack().(io.Writer)

	// Register tun2socks connection handlers.
	proxyAddr, err := net.ResolveTCPAddr("tcp", C.GoString(proxyServer))
	proxyHost := proxyAddr.IP.String()
	proxyPort := uint16(proxyAddr.Port)
	if err != nil {
		log.Info("invalid proxy server address: %v", err)
	}
	core.RegisterTCPConnHandler(socks.NewTCPHandler(proxyHost, proxyPort, nil))
	core.RegisterUDPConnHandler(socks.NewUDPHandler(proxyHost, proxyPort, 1*time.Minute, nil, nil))

	// Register an output callback to write packets output from lwip stack to tun
	// device, output function should be set before input any packets.
	core.RegisterOutputFn(func(data []byte) (int, error) {
		return tunDev.Write(data)
	})

	ctx, cancel = context.WithCancel(context.Background())

	// Copy packets from tun device to lwip stack, it's the main loop.
	go func(ctx context.Context) {
		_, err := io.CopyBuffer(lwipWriter, tunDev, make([]byte, MTU))
		if err != nil {
			log.Info(err.Error())
		}
	}(ctx)

	log.Info("Running tun2socks")

	isRunning = true

	<-ctx.Done()
}

//export startShadowsocksGo
func startShadowsocksGo(ClientAddr *C.char, ServerAddr *C.char, Cipher *C.char, Password *C.char, Plugin *C.char, PluginOptions *C.char) {
	shadowsocks.StartGoShadowsocks(C.GoString(ClientAddr), C.GoString(ServerAddr), C.GoString(Cipher), C.GoString(Password), C.GoString(Plugin), C.GoString(PluginOptions))
}

//export stopShadowsocksGo
func stopShadowsocksGo() {
	shadowsocks.StopGoShadowsocks()
}

//export startTrojanGo
func startTrojanGo(filename *C.char) {
	if client != nil {
		log.Info("Client is already running")
		return
	}
	log.Info("Running client, config file:", C.GoString(filename))
	configBytes, err := ioutil.ReadFile(C.GoString(filename))
	if err != nil {
		log.Error("failed to read file", err)
	}
	config, err := conf.ParseJSON(configBytes)
	if err != nil {
		log.Error("error", err)
		return
	}
	client, err = proxy.NewProxy(config)
	if err != nil {
		log.Error("error", err)
		return
	}
	go client.Run()
	log.Info("trojan launched")
	isTrojanGoRunning = true
}

//export stopTrojanGo
func stopTrojanGo() {
	if isTrojanGoRunning == true {
		log.Info("Stopping client")
		if client != nil {
			client.Close()
			client = nil
		}
		log.Info("Stopped")
		isTrojanGoRunning = false
	}
}

//export getTrojanGoVersion
func getTrojanGoVersion() *C.char {
	return C.CString(common.Version)
}

func main() {
}
