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
	"time"
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
			// Broadcast incoming message to all clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}
		case msg := <-priv:
			// command0 origin, command1 dest, command[2:] message
			command := strings.Split(msg, " ")
			usernameClient[command[1]] <-  command[0]+ " (priv to you): "+strings.Join(command[2:], " ")
			usernameClient[command[0]] <- "(you sent priv a to "+command[1]+"): " +strings.Join(command[2:], " ")
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
	//Scanning for messages from client.
	for input.Scan() {
		text  := input.Text()
		if len(text)==0 { continue }
		if text[0]!='/' { //Not a command.
			messages <- usernames[who] + ": " + text
			continue
		}
		//Command case.
		commandEvaluation(text, who, ch)
	}

	//User is desconected or  was kicked.
	leaving <- ch
	if usernames[who] !=""{
		sendMsgFromServer(usernames[who]  + " has left" ,messages)
	}
	delete(inverseUsernames, usernames[who] )
	delete(usernameClient, usernames[who] )

	if admin ==  usernames[who] {
		replaceAdmin()
	}

	if addressConnection[who] != nil {
		//User was not kicked, it left by itself and it should be disconnected.
		fmt.Printf("irc-server> [%s] left.\n", usernames[who])
		addressConnection[who].Close()
	}

	delete(usernames, who);
	delete(addressConnection, who)
}

func replaceAdmin(){
	if len(inverseUsernames)<=0{
		admin= ""
		return
	}
	for k,_ := range inverseUsernames {
		setAdmin(k, usernameClient[k], messages )
		return //we just need one user to be the admin, choosen "randomly" by the first in this for.
	}
}

func commandEvaluation( text string,cliAddr string, ch chan <- string){
	command := strings.Split(text, " ")
	switch command[0]{
	case "/setUser":
		if usernames[cliAddr ] != "" { return } /*This is for avoiding rechange your name once connected.
												This command is just an inner command for set username initially.*/
		setNewUser(command[len(command)-1], cliAddr ,  ch)
		entering <- ch
		if(len(usernames)==1){
			sendMsgFromServer("Congrats, you were the first user.",ch)
			setAdmin(usernames[cliAddr], ch, messages )
		}
	case "/user":
		for  i := 1;i<len(command);i++{		 
			if v, found := inverseUsernames[command[i]]; found {
				sendMsgFromServer("user: "+ command[i] +", address: "+v,ch)
			}
		}
	case "/msg":
		privMsg := usernames[cliAddr] +" "+ strings.Join(command[1:], " ")
		priv <- privMsg
	case "/users":
		msg:= "List of users: -- "
		for k,_ :=  range inverseUsernames{
			msg = msg + k +" -- "
		}
		sendMsgFromServer(msg,ch)
	case "/kick":
		if usernames[cliAddr] != admin {
			sendMsgFromServer("You cannot /kick anyone.You are not ADMIN." ,ch)
			return
		}
		for i:=1;i<len(command);i++{
			userToKick := command[i]
			if userToKick == usernames[cliAddr] { continue } //you cannot kick yourself.
			if _, found:= usernameClient[userToKick]; !found  { continue} // you cannot kick someone already kicked.
			kick(userToKick)
		}
	case "/time":
		msg:= "Local Time: "+ time.Now().Location().String() + " " +time.Now().Format("15:04 GMT-07")
		sendMsgFromServer(msg ,ch)
	default:
		messages <- usernames[cliAddr] + ": " + text
	}
}

func setNewUser(user, cliAddr  string,  ch chan <- string ){
	usernames[cliAddr ] = user
	inverseUsernames[user ] = cliAddr 
	usernameClient[user] = ch
	sendMsgFromServer("You: [" +  usernames[cliAddr] +"] are succesfully logged."  ,ch)
	fmt.Printf("irc-server> New connected user [%s].\n", usernames[cliAddr])
	sendMsgFromServer("[" + usernames[cliAddr] + "] has arrived.", messages)
}

func kick( user string){
	sendMsgFromServer("You were kicked",usernameClient[user])
	sendMsgFromServer("For bad behaviour\n",usernameClient[user])
	sendMsgFromServer(user+" was kicked",messages)
	fmt.Printf("irc-server> [%s] was kicked.\n", user)
	delete(usernameClient, user )
	delete(usernames, inverseUsernames[user]);
	addressConnection[inverseUsernames[user]].Close()
	delete(addressConnection, inverseUsernames[user])
	delete(inverseUsernames, user)
}

func setAdmin(ad string, ch chan <- string, messages chan <- string ){
	admin=  ad
	sendMsgFromServer(admin + " is now the ADMIN.", messages)
	fmt.Printf("irc-server> [%s] was promoted as the channel ADMIN.\n", admin)
}

func sendMsgFromServer(msg string,ch chan <- string){
	if ch!= nil{
		ch <- "irc-server: "+msg
	}
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	var host = flag.String("host", "localhost","Sets the ip of the server.")
	var port = flag.String("port", "8000","Sets the port of the server.")
	flag.Parse()
	var serverLocation = fmt.Sprintf("%s:%s", *host , *port)
	listener, err := net.Listen("tcp", serverLocation)
	initVars(*host, *port)

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

func initVars(host, port string){
	usernames = make(map[string]string)
	inverseUsernames = make(map[string]string)
	usernameClient = make(map[string]client)
	addressConnection= make(map[string]net.Conn)
	admin = ""
	fmt.Printf("irc-server>  Simple IRC Server started at %s:%s\n",host, port)
	fmt.Println("irc-server> Ready for receiving new clients.")
}

//!-main
