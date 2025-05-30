const {MongoClient} = require("mongodb");
const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const http = require('http');
const {Server} = require('socket.io');
const app = express();
const server = http.createServer(app);
const io = new Server(server);
const port = 3000;
let event = 0;
const url = "mongodb+srv://ashish4684:1P3awqKrKIVf02aU@iot-smartparking.g5z3i.mongodb.net/?retryWrites=true&w=majority&appName=IOT-SMARTPARKING";
const client = new MongoClient(url);
io.on("connection",async (socket_client)=>{
console.log(`a user connected with id: ${socket_client.id}`);
socket_client.emit("message","hi");
/*if(event==1)
{
try{
event = 0;
await client.connect();
const arrayOfslots = await findEmptySlots(client);
console.log("hello");
socket_client.emit("emptySlots",arrayOfslots);
}catch(error)
{
event = 0;
console.log("could not connect to database");
}finally
{
client.close();
}
}*/
});
/*const url = "mongodb+srv://ashish4684:1P3awqKrKIVf02aU@iot-smartparking.g5z3i.mongodb.net/?retryWrites=true&w=majority&appName=IOT-SMARTPARKING";
const client = new MongoClient(url);*/
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, '/')));
app.set('view engine', 'ejs');
/*async function main()
{
const url = "mongodb+srv://ashish4684:1P3awqKrKIVf02aU@iot-smartparking.g5z3i.mongodb.net/?retryWrites=true&w=majority&appName=IOT-SMARTPARKING";
const client = new MongoClient(url);
try{
await client.connect();
await listDatabases(client);
await insertUser(client,{rfid:"AB CD 08 56", balance:3000,entryTime:new Date()});
await findUser(client,"AB CD 08 56");
await updateBalance(client,"AB CD 08 56");
await findEmptySlots(client);
}catch(error)
{
console.log(error);
}
}*/
//main();
app.post('/rfid_data', async (req, res) => {
const userRfid = req.body.rfid;
console.log(userRfid);
/*res.status(200).send("data received successfully");*/
/*const url = "mongodb+srv://ashish4684:1P3awqKrKIVf02aU@iot-smartparking.g5z3i.mongodb.net/?retryWrites=true&w=majority&appName=IOT-SMARTPARKING";
const client = new MongoClient(url);*/
try{
await client.connect();
const result = await findUser(client,userRfid);
if(!result)res.status(201).send("not found");
else
{
const isuserinside = isUserInside(result);
if(!isuserinside)
{
await UserEnters(client,userRfid);
res.status(200).send("user is entering");
io.emit("start");
}
else
{
const balance = await userExits(client,userRfid);
res.status(202).send("user is exiting");
io.emit("exit",balance);
}
}
}catch(error)
{
res.send.status(500).send("could not connect to database");
}
finally{
client.close();
}
});
app.get("/login",(req,res)=>{
res.sendFile(path.join(__dirname, '/', 'modifiedLogin.html'));
})
app.post('/verify',async (req,res)=>{
const userName = req.body.username;
const passWord = req.body.password;
try{
await client.connect();
const user = await findUserForLogin(client,{username:userName,password:passWord});
if(!user)
{
res.status(200).send("user not found");
}
else
{
const emptySlots = await findEmptySlots(client);
const emptySlotId = [];
emptySlots.forEach((slot)=>{
emptySlotId.push(slot._id);
});
const dataTobeSent = {
username: user.username,
balance:user.balance,
emptyslots:emptySlotId
}
//res.render('dashboard', dataTobeSent);
res.render('modifiedDashboard2',dataTobeSent);
}
}catch(error)
{
console.log("cannot connect to database");
}
finally{
client.close();
}
});
app.post('/recharge',async (req,res)=>{
const username = req.body.username;
const amount = req.body.amount;
try{
await client.connect();
await Recharge(client,username,amount);
res.status(200).json({msg:"recharge done",balance:amount});
}catch(error)
{
console.log("could not connect to database");
res.status(500).send("could not connect to database");
}finally{
await client.close();
}
});
async function Recharge(client,userName,balance)
{
const result = await
client.db('smart_parking').collection('prepaid_users').updateOne({username:userName},{$set:{balance:balance}});
console.log(result);
}
function isUserInside(result)
{
if(result.isinside==="no")return false;
else return true;
}
app.post('/emptyslotsdata',async(req,res)=>{
const slot = req.body;
const occupationStatus = slot.occupation_status==1?"occupied":"unoccupied";
console.log(slot);
await client.connect();
// event = 1;
const arrayOfSlots = await findEmptySlots(client);
io.emit("emptySlots",arrayOfSlots);
try{
const result = await
client.db('smart_parking').collection('slots').updateOne({_id:slot._id},{$set:{occupation_status:occupationStatus}});
const arrayOfSlots = await findEmptySlots(client);
const slotid=[];
arrayOfSlots.forEach((slots)=>{
slotid.push(slots._id);
});
console.log(slotid);
io.emit("emptySlots",slotid);
res.status(200).send("empty slot details updated successfully");
}catch(error)
{
console.log("could not connect to database");
res.status(500).send("could not connect to database");
}finally{
client.close();
}
});
async function findUserForLogin(client,query)
{
const result = await client.db("smart_parking").collection("prepaid_users").findOne(query);
console.log(result);
return result;
}
async function listDatabases(client)
{
const dataBases = await client.db().admin().listDatabases();
console.log("DataBases - ");
dataBases.databases.forEach(db=>console.log(db.name));
}
async function insertUser(client,userDetails)
{
const result = await
client.db("smart_parking").collection("prepaid_users").insertOne(userDetails);
console.log(result);
}
async function findUser(client,userRfid)
{
const result = await
client.db("smart_parking").collection("prepaid_users").findOne({rfid:userRfid});
/*if(!result)console.log("user does not exists");
else console.log(result);*/
return result;
}
function calculateCharge(result)
{
const chargepermin = 20;
let entryTime = result.entryTime.getTime();
let exitTime = new Date().getTime();
const diffInMs = exitTime - entryTime;
const hours = diffInMs/(1000*3600);
const mins = diffInMs/(1000*60);
const sec = diffInMs/1000;
let totalCharge = chargepermin * mins;
let remainingBalance = result.balance - totalCharge;
return remainingBalance;
}
async function userExits(client,userRfid)
{
const result = await
client.db("smart_parking").collection("prepaid_users").findOne({rfid:userRfid});
let remBalance = calculateCharge(result);
//io.emit('remainingbalance',remBalance);
const result2 = await
client.db("smart_parking").collection("prepaid_users").updateOne({rfid:userRfid},{$set:{balance:remBalance,isinside:"no"}});
return remBalance;
}
async function findEmptySlots(client)
{
const cursor = await
client.db("smart_parking").collection("slots").find({occupation_status:"unoccupied"});
const arrayOfSlots = await cursor.toArray();
//show empty slot on web page
arrayOfSlots.forEach(emptyslots=>console.log(`slot ${emptyslots._id} is empty`));
return arrayOfSlots;
}
async function UserEnters(client,userRfid)
{
const result = await
client.db("smart_parking").collection("prepaid_users").updateOne({rfid:userRfid},{$set:{entryTime:new Date(),isinside:"yes"}});
console.log(result);
}
server.listen(port,"0.0.0.0", () => {
console.log(`Server running on port ${port}`);
});