import logo from './logo.svg';
import {useState,useEffect} from 'react';
import './App.css';
import {io} from 'socket.io-client';
import portNo from "./serverport"
function App() {
    let [ws,setWs]= useState(null);
    let [count,setCount] = useState(0);
    let [status,setStatus] = useState("");
      // ws.onopen = (event) => {
      //   ws.send(JSON.stringify("Hi there"));
      // };
    let [pingingIntervalId,setPingingIntervalId] = useState(null);
    async function init(){
      // get the port number from ../serverport
      let port = await (await fetch(portNo)).text()
      console.log(port)
      setWs(new WebSocket(`ws://localhost:${port}/ws`))
      
    }
    useEffect(()=>{
      if(ws)
      ws.onmessage = function (event) {
        console.log('Message from server ', event.data);
        if(event.data==="end"){
          setStatus("end");
        }
        else
        setCount(event.data);
      };
    },[ws])
    useEffect(() => {
      init();
    }, []);

    useEffect(()=>{

      if(status=="start"){
        sendData("start")
        setStatus("ping");
      }
      else if(status=="ping"){
       let id =  setInterval(()=>{
          sendData("ping");
          }, 3);
        setPingingIntervalId(id);
      }
      else if(status=="end"){
        console.log("Ending pinging ")
        if(pingingIntervalId)
          clearInterval(pingingIntervalId)
        
      }
      else if(status=="kill"){
        sendData("kill")
      }
    },[status])

  function sendData(text){
      if(ws )
      {
          ws.send(text);
      }
      else
      {
        console.log("trying to send ",text)
          setTimeout(()=>sendData(text), 1000);
      }
  }

  const handleOnCLickVisualize = ()=>{
    setStatus("start");
  }
  const handleOnClickStop = () =>{
    setStatus("kill")
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
