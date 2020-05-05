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
	"bufio"
	"strings"
)

var user = flag.String("user", "randomUser", "Sets username.")
var server = flag.String("server", "localhost:8000","Sets the ip:port of the server.")

//!+
func main() {

	flag.Parse()
	conn, err := net.Dial("tcp", *server)
	if err != nil {
		log.Fatal(err)
	}

	done := make(chan struct{})
	var setUserComm = fmt.Sprintf("/setUser %s\n" , *user )
	
	if _, err := io.WriteString(conn, setUserComm); err != nil {
		log.Fatal(err)
	}
	go func() {
		input := bufio.NewScanner(conn)
		for input.Scan() {
			toPrint := input.Text()
			if toPrintArr:= strings.Split(toPrint, ":"); toPrintArr[0] !=*user{
				fmt.Print("\n")		
			}
			fmt.Print(input.Text())
			fmt.Print("\n"+*user + "> ")
		}
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		done <- struct{}{} // signal the main goroutine
	}()

	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	// writing
	output := bufio.NewScanner(src)
	for output.Scan() {
		if output.Text() == "" {
			fmt.Print(*user + "> ")
			continue
		}
		_, e := io.WriteString(dst, output.Text()+"\n")
		if e != nil {
			fmt.Printf("Connection closed\n")
			return
		}
	}
}