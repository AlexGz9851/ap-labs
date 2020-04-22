// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"io"
	"log"
	"net"
	"time"
	"flag"
	"strconv"
)

func handleConn(c net.Conn, locationName string) {
	defer c.Close()
	
	for {
		loc, err := time.LoadLocation(locationName);
		if err != nil {
			return 
		}
		_, err2 := io.WriteString(c, time.Now().In(loc).Format("15:04:05"))
		if err2 != nil {
		
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	var location = flag.String("TZ", "US/Eastern", "Sets a specific timezone for a server.")
	var port = flag.Int("port", 123,"Sets a specific port for a server in given timezone.")
	flag.Parse()
	var portStr = strconv.Itoa(*port)
	listener, err := net.Listen("tcp", "localhost:" + portStr)
	if err != nil {
		log.Fatal(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn, *location) // handle connections concurrently
	}
}
