// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"flag"
	"strings"
)

var usernames map[string]string
var inverseUsernames map[string]string // key: username , value: address
var usernameClient map[string]client
var addressConnection map[string]net.Conn
var admin string

//!+broadcaster
type client chan<- string // an outgoing message channel

var (
	entering = make(chan client)
	leaving  = make(chan client)
	messages = make(chan string) // all incoming client messages
	priv = make(chan string) // priv msgs channel
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}
		case msg := <-priv:
			//search in all users till get the correct one
			// command0 origin, command1 dest, command[2:] message
			command := strings.Split(msg, " ")
			for username, cli := range usernameClient {
				if(username == command[1]){
					privMsg := command[0]+ " (priv to you): "
					for i:=2;i<len(command); i++{
						privMsg = privMsg + " " + command[i]
					}
					cli <- privMsg
				}else if(username == command[0]){
					privMsg := "(you sent priv a to "+command[1]+"): "
					for i:=2;i<len(command); i++{
						privMsg = privMsg + " " + command[i]
					}
					cli <- privMsg
				}
				
			}
		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			close(cli)
			delete(clients, cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	who := conn.RemoteAddr().String()

	input := bufio.NewScanner(conn)
	for input.Scan() {
		text  := input.Text()
		if len(text)==0 { continue }
		if text[0]!='/' {
			messages <- usernames[who] + ": " + text
			continue
		}
		command := strings.Split(text, " ")
		switch command[0]{
		case "/setUser":
			if usernames[who] != "" { break } 
			usernames[who] = command[len(command)-1]
			inverseUsernames[usernames[who] ] = who
			usernameClient[usernames[who]] = ch
			sendMsgFromServer("You are " +  usernames[who]  ,ch)
			sendMsgFromServer(usernames[who] + " has arrived.", messages)
			entering <- ch
			if(len(usernames)==1){
				setAdmin(usernames[who], ch, messages )
			}
		case "/user":
			for  i := 1;i<len(command);i++{
				v, found := inverseUsernames[command[i]] 
				if found {
					msg := "user: "+ command[i] +", address: "+v
					sendMsgFromServer(msg ,ch)
				}
			}
		case "/msg":
			privMsg := usernames[who]
			for i:=1;i<len(command); i++{
				privMsg = privMsg + " " + command[i]
			}
			priv <- privMsg
		case "/users":
			for k,v :=  range inverseUsernames{
				msg:= "user: "+ k +", address: "+v
				sendMsgFromServer(msg ,ch)
			}
		case "/kick":
			if(usernames[who] == admin){
				for i:=1;i<len(command);i++{
					if command[i]== usernames[who] { continue }
					for user, _:= range usernameClient{
						if(command[i] != user){ continue }
						sendMsgFromServer(user+" was kicked",messages)
						sendMsgFromServer("You were kicked",usernameClient[user])
						//kick(user)
					}
				}
			}else{
				sendMsgFromServer("You cannot /kick anyone.You are not ADMIN." ,ch)
			}
		default:
			messages <- usernames[who] + ": " + text
		}
	}
	sendMsgFromServer(usernames[who]  + " has left" ,messages)
	leaving <- ch
	delete(inverseUsernames, usernames[who] )
	delete(usernameClient, usernames[who] )
	if admin ==  usernames[who] {
		if len(inverseUsernames)>0{
			for k,_ := range inverseUsernames {
				setAdmin(k, usernameClient[k], messages )
				break //we just need one user to be the admin, choosen "randomly" by the first in this for.
			}
		}else{
			admin= ""
		}
	}
	addressConnection[who].Close()
	delete(usernames, who);
	delete(addressConnection, who)
}

func setAdmin(ad string, ch chan <- string, messages chan <- string ){
	admin=  ad
	sendMsgFromServer(admin + " is now the ADMIN.", messages)
}

func sendMsgFromServer(msg string,ch chan <- string){
	ch <- "server> "+msg
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn
func initVars(){
	usernames = make(map[string]string)
	inverseUsernames = make(map[string]string)
	usernameClient = make(map[string]client)
	addressConnection= make(map[string]net.Conn)
	admin = ""
}

//!+main
func main() {
	var host = flag.String("host", "localhost","Sets the ip of the server.")
	var port = flag.String("port", "8000","Sets the port of the server.")
	flag.Parse()
	var serverLocation = fmt.Sprintf("%s:%s", *host , *port)
	listener, err := net.Listen("tcp", serverLocation)
	initVars()

	if err != nil {
		log.Fatal(err)
	}

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		addressConnection[conn.RemoteAddr().String()] = conn
		go handleConn(conn)
	}
}

//!-main
