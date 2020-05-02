// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"fmt"
	"flag"
	"math/rand"
)

//!+
func main() {
	var random = rand.Intn(1000)
	var randomUser = fmt.Sprintf("%s%d", "user" , random)
	var user = flag.String("user", randomUser, "Sets username.")
	var server = flag.String("server", "localhost:8000","Sets the ip:port of the server.")
	flag.Parse()
	conn, err := net.Dial("tcp", *server)
	if err != nil {
		log.Fatal(err)
	}

	done := make(chan struct{})
	var setUserComm = fmt.Sprintf("%s%s%s", "/setUser " , *user,"\n" )
	
	if _, err := io.WriteString(conn, setUserComm); err != nil {
		log.Fatal(err)
	}
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		//log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()

	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
