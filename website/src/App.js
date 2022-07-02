import logo from './logo.svg';
import {useState,useEffect} from 'react';
import './App.css';
import {io} from 'socket.io-client';
import portNo from "./serverport"
function App() {
  let ws;
  let [count,setCount] = useState(0);

 
    // ws.onopen = (event) => {
    //   ws.send(JSON.stringify("Hi there"));
    // };
    
    async function init(){
      

      // get the port number from ../serverport
      let port = (await fetch(portNo)).text()
  
      ws = new WebSocket(`ws://localhost:${port}/ws`);
      ws.onmessage = function (event) {
  
        try {
                console.log(event.data);
            setCount(Number(event.data))
          } catch (err) {
            console.log(err);
          } 
        };
    }
  useEffect(() => {
      
  }, []);

  function sendData(text)
  {
    console.log("sending. . . ")
      if(ws && ws.readyState)
      {
          ws.send(text);
      }
      else
      {
        console.log("trying to send ")
          setTimeout(sendData, 1000);
      }
  }

  const handleOnCLickVisualize = ()=>{
    sendData("Start");
  }
  const handleOnClickStop = () =>{
    sendData("kill")
  }
  
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
          {count}
        </a>
        <button onClick={handleOnCLickVisualize}>Visualize</button>
        <button onClick={handleOnClickStop}>Stop</button>

      </header>
    </div>
  );
}

export default App;
