import { useState, useEffect } from 'react';
import './App.css';
import playicon from './play_icon.png';
import ritlogo from './rit_logo.png';

import { io } from 'socket.io-client';
import portNo from "./serverport" 
//import * as Highcharts from "highcharts";
import Highcharts from 'highcharts';
import HighchartsReact from 'highcharts-react-official';
import highchartsMore from "highcharts/highcharts-more";
import solidGauge from "highcharts/modules/solid-gauge";
import Modal from 'react-modal';
 
function App() {
  const [data, setData] = useState([0]);
  const [modalIsOpen, setIsOpen] = useState(false); 
  const [options, setOptions] = useState({  
    chart: {
      type: 'solidgauge'
    },
    title: {
      text: 'Average'
    },
    yAxis: {
        min: 0,
        max: 3000000000,
    },
    pane: {
      center: ['50%', '85%'],
      size: '140%',
      startAngle: -90,
      endAngle: 90,
      background: {
        backgroundColor:
          '#1E1F24',
        innerRadius: '60%',
        outerRadius: '100%',
        shape: 'arc' 
      }
    },  
    series: [{
      name: 'Avg Value',
      data: data
    }]
  });
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
      console.log(data); 
      setOptions((prevState) => {
        let updatedOptions = Object.assign({}, options);
        updatedOptions.series[0].data = data;
        return updatedOptions;
      }) 
    },[data])

    useEffect(()=>{
      if(ws)
      ws.onmessage = function (event) {
        console.log('Message from server ', event.data);
        if(event.data==="end"){
          setStatus("end");
        }   
        else{
            setCount(event.data);
            setData([parseInt(event.data)]);  
        }
       
      };
  }, [ws])

  useEffect(() => {
    highchartsMore(Highcharts);
    solidGauge(Highcharts);
    init();
  }, []);

  useEffect(() => {
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

  const handleUpdateGraph = (c) => { 
    /*for (let i = 0; i < 10; ++i) {
      let prevData = data;
      prevData.push(i);
      setData([prevData]); 
    }*/
    setData([c]); 
    console.log(data);
    setOptions((prevState) => {
      let updatedOptions = Object.assign({}, options);
      updatedOptions.series[0].data = data;
      return updatedOptions;
    })
    // console.log(options);
  }

  function openModal() {
    setIsOpen(true);
  }

  function closeModal() {
    setIsOpen(false);
  }

  const customStyles = {

    content: {
      top: '50%',
      left: '50%',
      right: 'auto',
      bottom: 'auto',
      transform: 'translate(-50%, -50%)',
    },
  };

  return (
    <div className="App">
      <header className="App-header">
        <img src={ritlogo} alt="RIT" id='rit-logo'></img>
        <button onClick={handleOnCLickVisualize} style={{ top: '35%' }}><img src={playicon} alt="play"></img></button>
        <button onClick={handleOnClickStop} style={{ top: '60%' }}>Stop</button>
        <button id="info-button" onClick={openModal}>?</button>
        <div style={{ display: 'flex', flexDirection: 'row', height: '100vh', width: "100%" }}>
          <div className="Column" style={{ borderRight: "1px solid #95afc0" }}>
            <div className='column-title'>Parallel Iteration + Progressive visualization</div>
            <div className='visualization-box'>
              <HighchartsReact highcharts={Highcharts} options={options} />
            </div>
          </div>
          <div className="Column" style={{ borderLeft: "1px solid #95afc0" }}>
            <div className='column-title'>Traditional querying</div>
            <div className='visualization-box'>{count}<p>is the average computed</p></div>
          </div>
        </div>
        <div id='credits' onClick={openModal}>Made with ❤ for mini-project 2022</div>
        <Modal
          style={customStyles}
          isOpen={modalIsOpen}
          onRequestClose={closeModal}
          contentLabel="Example Modal"
        >  
          <p>What is this project about?<br></br>This is a research-based project where we work on unique algorithms to give the best possible experience to the end user while exploring huge databases: both in terms of <em>speed</em> and <em>interactivity</em>. On the left is our new tech; on the right is the traditional mechanism</p>
          <p>How are we doing this?<br></br>This project uniquely combines parallel iterative processing with progressive visualization to find out how the querying process can be enhanced. The backend is where much of the magic happens: our uniquely designed algorithms, with custom data structures coded from scratch.</p>
          <p style={{ color: "#636E72" }}>Made by Sahil, Aakash, Soundarya and Subinoy</p>
        </Modal>
      </header >
    </div >
  );
} 

export default App;
