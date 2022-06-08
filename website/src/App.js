import logo from './logo.svg';
import {useState,useEffect} from 'react';
import './App.css';
import {io} from 'socket.io-client';
function App() {
  const ws = new WebSocket("ws://localhost:18080/ws");

  ws.onmessage = function (event) {

    try {
            console.log(event.data);
        
      } catch (err) {
        console.log(err);
      } 
    };
    
    ws.onopen = (event) => {
      ws.send(JSON.stringify("Hi there"));
    };
    
    useEffect(() => {
  }, []);
  
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p> 
          Edit <code>src/App.js</code> and save to reload.
        </p>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
      </header>
    </div>
  );
}

export default App;
