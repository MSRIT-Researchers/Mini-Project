const express = require('express');
const app = express();
const http = require('http');
const server = http.createServer(app);
const { Server } = require("socket.io");
const io = new Server(server,{
    cors: {
        origin: "http://localhost:3000"
      }
});

app.get('/', (req, res) => {
  res.sendFile("Hello");
});   

io.on('connection', (socket) => {
  console.log('a user connected');
  socket.emit("message", "Hello World from socket io server");
});

server.listen(3001, () => {
  console.log('listening on *:3001');
});
