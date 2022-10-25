const express = require("express")
const app = express()
const path= require("path")
const { plot } =require('nodeplotlib')
const rp = require('request-promise');
var dateTime = require('node-datetime');
const url = "https://weather-station.crazykaenguru.repl.co/"//'http://192.168.2.130/';
const fs = require('fs')
var T;
var P;
const requestdelay=60000;
app.set('views', path.join(__dirname, 'views'));
    app.set('view engine', 'ejs');
// Handling GET / request
app.use("/", async(req, res, next) => {
    var data = fs.readFileSync('./data.txt', 'utf8')
    data=data.toString()
    data=data.split("\n")
    var currentdata= data[data.length-2]
    var allT=[];
    var alltime=[];
    currentdata=currentdata.split("_")
    console.log(currentdata)
    T=currentdata[0] 
    P=currentdata[1]
    for(var i=0;i<data.length;i++)
    {
      allT.push((data[i]).toString().split("_")[0])
    }
    for(var i=0;i<data.length;i++)
    {
      if((data[i]).toString().split("_")[2]!=undefined)
      {
      alltime.push((data[i]).toString().split("_")[2])
      }
    }
   // allT=allT.slice(-10)
   // console.log(allT)
   //console.log(allT)
    res.header(2)
   // res.send("Temperature: "+T+" Pressure: "+P)
   console.log(alltime)
   res.render('index',{temp:allT,time:alltime});
    
})
  
// Handling GET /hello request
app.get("/hello", (req, res, next) => {
    res.header(2)
    res.send("This is the hello response");
    
})

    setInterval(function(){
      //   console.log("hi")
        getData()},requestdelay)
 async function getData()
{
 //fetch("http://192.168.2.130/")
 await rp(url)
 .then(  async function(html){
   //success!
  // console.log(html);
   var dt = dateTime.create();
   var formatted = dt.format('Y-m-d H:M:S');
   
   var dataset = `${html.toString().split("_")[0]}_${html.toString().split("_")[1].split('\r')[0]}_${formatted}\n`
  
 
 // data= JSON.parse(data)
  
  fs.appendFileSync('./data.txt', dataset, 'utf8')
 })
 .catch(function(err){
   //handle error
 });
 
}
  
// Server setup
app.listen(process.env.PORT||3000, () => {
    console.log("Server is Running")
})