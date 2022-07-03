import { useState, useEffect } from 'react';
import './App.css';
import playicon from './play_icon.png';
import { io } from 'socket.io-client';
import portNo from "./serverport"
//import * as Highcharts from "highcharts";
import Highcharts from 'highcharts/highstock';
import HighchartsReact from 'highcharts-react-official';

function App() {

  const options = {
    title: {
      text: 'My stock chart'
    },
    series: [
      {
        data: [1, 2, 1, 4, 3, 6, 7, 3, 8, 6, 9]
      }
    ]
  };


  let [ws, setWs] = useState(null);
  let [count, setCount] = useState(0);
  //HERE
  /*let [chartCount, setChartCount] = useState(
    Highcharts.chart('progressive-visualization', {
      chart: {
        type: 'solidgauge'
      },

      title: null,

      yAxis: {
        min: 0,
        max: 200,
        title: {
          text: 'Count'
        }
      },

      credits: {
        enabled: false
      },

      series: [{
        name: 'Records count',
        data: [0],
        dataLabels: {
          format:
            '<div style="text-align:center">' +
            '<span style="font-size:25px">{y}</span><br/>' +
            '<span style="font-size:12px;opacity:0.4">records</span>' +
            '</div>'
        },
        tooltip: {
          valueSuffix: ' records'
        }
      }]

    })
  );*/
  let [status, setStatus] = useState("");
  // ws.onopen = (event) => {
  //   ws.send(JSON.stringify("Hi there"));
  // };

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
        //HERE
        /*setChartCount(Highcharts.chart('progressive-visualization', {
          chart: {
            type: 'solidgauge'
          },

          title: null,

          yAxis: {
            min: 0,
            max: 200,
            title: {
              text: 'Count'
            }
          },

          credits: {
            enabled: false
          },

          series: [{
            name: 'Records count',
            data: [event.data],
            dataLabels: {
              format:
                '<div style="text-align:center">' +
                '<span style="font-size:25px">{y}</span><br/>' +
                '<span style="font-size:12px;opacity:0.4">records</span>' +
                '</div>'
            },
            tooltip: {
              valueSuffix: ' records'
            }
          }]

        }));*/
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


  var chartCount = Highcharts.chart('progressive-visualization', {

    yAxis: {
      min: 0,
      max: 200,
      title: {
        text: 'Count'
      }
    },

    credits: {
      enabled: false
    },

    series: [{
      name: 'Records count',
      data: [80],
      dataLabels: {
        format:
          '<div style="text-align:center">' +
          '<span style="font-size:25px">{y}</span><br/>' +
          '<span style="font-size:12px;opacity:0.4">records</span>' +
          '</div>'
      },
      tooltip: {
        valueSuffix: ' records'
      }
    }]

  });


  setInterval(function () {
    var point,
      newVal,
      inc;


    point = chartCount.series[0].points[0];
    inc = Math.round((Math.random() - 0.5) * 100);
    newVal = point.y + inc;

    if (newVal < 0 || newVal > 200) {
      newVal = point.y - inc;
    }

    point.update(newVal);

  }, 2000);

  return (
    <div className="App">
      <header className="App-header">
        <button onClick={handleOnCLickVisualize} style={{ top: '35%' }}><img src={playicon} alt="play"></img></button>
        <button onClick={handleOnClickStop} style={{ top: '60%' }}>Stop</button>
        <div style={{ display: 'flex', flexDirection: 'row', height: '100vh', width: "100%" }}>
          <div className="Column" style={{ borderRight: "1px solid #95afc0" }}>
            <HighchartsReact
              highcharts={Highcharts}
              constructorType={'stockChart'}
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
