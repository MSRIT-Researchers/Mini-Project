import { useState, useEffect } from 'react';
import './App.css';
import ritlogo from './assests/rit_logo.png';
import loading from './assests/loading.gif'
import { io } from 'socket.io-client';
import portNo from "./serverport"
import Highcharts from 'highcharts';
import HighchartsReact from 'highcharts-react-official';
import highchartsMore from "highcharts/highcharts-more";
import solidGauge from "highcharts/modules/solid-gauge";
import Modal from 'react-modal';

function App() {
  highchartsMore(Highcharts);
  solidGauge(Highcharts);
  const [pingingIntervalId, setPingingIntervalId] = useState(null);
  const [ws, setWs] = useState(null);
  const [count, setCount] = useState(0);
  const [isMultiThreading,setIsMultiThreading] = useState(false)
  const [status, setStatus] = useState("");
  const [delay, setDelay] = useState(0);
  const [data, setData] = useState([0]);
  const [modalIsOpen, setIsOpen] = useState(false);
  const [runSingle, setRunSingle] = useState(false);
  const [isLoading, setIsLoading] = useState(false);
  const [options, setOptions] = useState({
    chart: {
      type: 'solidgauge',
      backgroundColor:
        '#37393F',
    },
    title: {
      text: ''
    },
    yAxis: {
      stops: [
        [0.5, '#FFBFA4'],
        [0.5, '#F8EFBA']
      ],
      min: 0,
      max: 200000,
    },
    pane: {
      center: ['50%', '85%'],
      size: '140%',
      startAngle: -90,
      endAngle: 90,
      background: {
        backgroundColor:
          '#37393F',
        innerRadius: '60%',
        outerRadius: '100%',
        shape: 'arc'
      }
    },
    series: [{
      name: 'Avg Value',
      data: data,
      animation: false
    }]
  });
  
  async function init() {
    // get the port number from ../serverport
    let port = await (await fetch(portNo)).text()
    console.log(port)
    setWs(new WebSocket(`ws://localhost:${port}/ws`))
  }

  useEffect(() => {
    init();
  }, []);

  useEffect(() => {
    if (delay === 100) {
      setOptions((prevState) => {
        let updatedOptions = Object.assign({}, options);
        updatedOptions.series[0].data = data;
        return updatedOptions;
      })
      setDelay(0);
    }
  }, [delay]);

  useEffect(() => {
    if (ws)
      ws.onmessage = function (event) {
        console.log('Message from server ', event.data);
        if (event.data === "end") {
          setStatus("end");
        }
        else {
          setIsLoading(false)
          setCount(event.data);
          setData([parseInt(event.data)]);
          setDelay(prevDelay => {
            return prevDelay + 1;
          });
        }
      };
  }, [ws])

  useEffect(() => {
    if (status == "start") {
        sendData("start")
        setStatus("ping");
    }
    else if (status == "ping") {
      let id = setInterval(() => {
        sendData("ping");
      }, 3);
      setPingingIntervalId(id);
    }
    else if(status=="startSingle"){
      sendData("startSingle");
    }
    else if (status == "end") {
      console.log("Ending pinging ")
      if (pingingIntervalId)
        clearInterval(pingingIntervalId)
      setOptions((prevState) => {
          let updatedOptions = Object.assign({}, options);
          updatedOptions.series[0].data = data;
          return updatedOptions;
        })
    }
    else if (status == "kill") {
      sendData("kill")
    }
  }, [status])

  useEffect(()=>{
    if(runSingle){
      setIsLoading(false);
      setRunSingle(false);
    }
  },[count])

  const sendData = (text)=>{
    if (ws) {
      ws.send(text);
    }
    else {
      console.log("trying to send ", text)
      setTimeout(() => sendData(text), 1000);
    }
  }

  const handleOnCLickVisualize = () => {
    setIsMultiThreading(true);
    setStatus("start");
  }

  const handleOnCLickVisualizeSingle = ()=>{
    setIsMultiThreading(false);
    setRunSingle(true);
    setIsLoading(true);
    setStatus("startSingle");
  }
  const handleOnClickStop = () => {
    setStatus("kill")
  }

  const openModal = ()=>{
    setIsOpen(true);
  }

  const closeModal = ()=>{
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
      <span style={{color:"white",paddingTop:"10px"}}>
        Average Online Fraud Amount
        </span>
        <img src={ritlogo} alt="RIT" id='rit-logo'></img>

        <button id="info-button" onClick={openModal}>?</button>
        <div style={{ display: 'flex', flexDirection: 'row', height: '100vh', width: "100%" }}>
          <div className="Column column-left">
            <div className='column-title'>Parallel Iteration + Progressive visualization</div>
            <div className='visualization-box'>
              <HighchartsReact highcharts={Highcharts} options={options} />
            </div>
            <div style={{color:"white",marginBottom:20}}>{!isMultiThreading?0:count}</div>
            <button onClick={handleOnCLickVisualize} style={{ top: '35%', color: "#96D391", fontSize: "30px" }}>▶</button>
          </div>
          <div className="Column column-right">
            <div className='column-title'>Traditional querying</div>
            {isLoading?<img src={loading} alt='loading' id='loading'/>:
            <div className='visualization-box'>{isMultiThreading?0:count}<p>is the average computed</p></div>}
            <button onClick={handleOnCLickVisualizeSingle} style={{ top: '35%', color: "#96D391", fontSize: "30px" }}>▶</button>
          </div>
        </div>
        <div id='credits' onClick={openModal}>Made with ❤ for mini-project 2022</div>
        <Modal
          style={customStyles}
          isOpen={modalIsOpen}
          onRequestClose={closeModal}
          contentLabel="Example Modal"
        >
          <p style={{ color: "black" }}>What is this project about?<br></br>This is a research-based project where we work on unique algorithms to give the best possible experience to the end user while exploring huge databases: both in terms of <em>speed</em> and <em>interactivity</em>. On the left is our new tech; on the right is the traditional mechanism</p>
          <p style={{ color: "black" }}>How are we doing this?<br></br>This project uniquely combines parallel iterative processing with progressive visualization to find out how the querying process can be enhanced. The backend is where much of the magic happens: our uniquely designed algorithms, with custom data structures coded from scratch.</p>
          <p style={{ color: "#636E72" }}>Made by Sahil, Aakash, Soundarya and Subinoy, RIT</p>
        </Modal>
      </header >

    </div >
  );
}

export default App;
