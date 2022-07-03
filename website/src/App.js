import { useState, useEffect } from 'react';
import './App.css';
import playicon from './play_icon.png';
import { io } from 'socket.io-client';
import portNo from "./serverport"
//import * as Highcharts from "highcharts";
import Highcharts from 'highcharts';
import HighchartsReact from 'highcharts-react-official';

function App() {

  const [data, setData] = useState([]);
  const [options, setOptions] = useState({
    title: {
      text: 'My chart'
    },
    series: [{
      data: data
    }]
  });

  let [ws, setWs] = useState(null);
  let [count, setCount] = useState(0);
  
  let [status, setStatus] = useState("");

  async function init() {
    // get the port number from ../serverport
    let port = await (await fetch(portNo)).text()
    console.log(port)
    setWs(new WebSocket(`ws://localhost:${port}/ws`))

  }
  useEffect(() => {
    if (ws)
      ws.onmessage = function (event) {
        console.log('Message from server ', event.data);
        setCount(event.data);
       
      };
  }, [ws])
  useEffect(() => {
    init();
  }, []);

  useEffect(() => {

    if (status === "start") {
      sendData("start")
      setStatus("ping");
    }
    else if (status === "ping") {
      setInterval(() => {
        sendData("ping");
      }, 2);
    }
    else if (status === "kill") {
      sendData("kill")
    }
  }, [status])

  function sendData(text) {
    if (ws) {
      ws.send(text);
    }
    else {
      console.log("trying to send ", text)
      setTimeout(() => sendData(text), 1000);
    }
  }

  const handleOnCLickVisualize = () => {
    setStatus("start");
  }
  const handleOnClickStop = () => {
    setStatus("kill")
  }

  const handleUpdateGraph = ()=>{
    for(let i=0; i<10; ++i){
        let prevData = data;
        prevData.push(i);
        setData([prevData]);
    }
    // console.log(data);
    setOptions((prevState)=>{
      let updatedOptions = Object.assign({},options);
      updatedOptions.series[0].data = data;
      return updatedOptions; 
    })
    // console.log(options);
  }

  return (
    <div className="App">
      <header className="App-header">
        <button onClick={handleOnCLickVisualize} style={{ top: '35%' }}><img src={playicon} alt="play"></img></button>
        <button onClick={handleOnClickStop} style={{ top: '60%' }}>Stop</button>
        <button onClick={handleUpdateGraph}> Update Graph</button>
        <div style={{ display: 'flex', flexDirection: 'row', height: '100vh', width: "100%" }}>
          <div className="Column" style={{ borderRight: "1px solid #95afc0" }}>
            <HighchartsReact
              highcharts={Highcharts}
              options={options}
            />
          </div>
          <div className="Column" style={{ borderLeft: "1px solid #95afc0" }}>{count}</div>
        </div>

      </header >
    </div >
  );
}

export default App;
