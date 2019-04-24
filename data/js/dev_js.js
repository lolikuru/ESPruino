var xmlHttp=createXmlHttpObject();
var switch_pos = [0,0,0,0,0,0,0,0];
var fadeon = [0,0,0];
var bright = [0,0,0];
var amount =[0,0,0];
function createXmlHttpObject(){
 if(window.XMLHttpRequest){
  xmlHttp=new XMLHttpRequest();
 }else{
  xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
 }
 return xmlHttp;
}
function load(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
  xmlHttp.open('PUT','/configs.json',true);
  xmlHttp.send(null);
  xmlHttp.onload = function(e) {
   jsonResponse=JSON.parse(xmlHttp.responseText);
   loadBlock();
  }
 }
}

function loadBlock(data2) {
	data2 = JSON.parse(xmlHttp.responseText);
	data = document.getElementsByTagName('body')[0].innerHTML;
	var new_string;
	for (var key in data2)
		{if(isFinite(data2[key])==false){
		new_string = data.replace(new RegExp('{{'+key+'}}', 'g'), data2[key][0]);
		data = new_string;}
		else {
		new_string = data.replace(new RegExp('{{'+key+'}}', 'g'), data2[key]);
		data = new_string;}
	}
	document.getElementsByTagName('body')[0].innerHTML = new_string;
	handleServerResponse();
	load_presset();
	last();
}

function load_presset(){
datas = JSON.parse(xmlHttp.responseText);
for (var i = 0;i < 8; i++) {
switch_pos[i]=datas["pin"+i][1];
}
for (var i = 0;i < 3; i++) {
fadeon[i]=datas["fadeon"+i];
bright[i]=datas["bright"+i];
amount[i]=datas["amount"+i];
}
}

function val(id){
 var v = document.getElementById(id).value;
 return v;
}
function send_request(submit,server){
 request = new XMLHttpRequest();
 request.open("GET", server, true);
 request.send();
 save_status(submit,request);
}
function save_status(submit,request){
 old_submit = submit.value;
 request.onreadystatechange = function() {
  if (request.readyState != 4) return;
  submit.value = request.responseText;
  setTimeout(function(){
   submit.value=old_submit;
   submit_disabled(false);
  }, 1000);
 }
 submit.value = 'Подождите...';
 submit_disabled(true);
}
function submit_disabled(request){
 var inputs = document.getElementsByTagName("input");
 for (var i = 0; i < inputs.length; i++) {
  if (inputs[i].type === 'submit') {inputs[i].disabled = request;}
 }
}
function toggle(target) {
 var curVal = document.getElementById(target).className;
 document.getElementById(target).className = (curVal === 'hidden') ? 'show' : 'hidden';
}

function last(){
sw_load(document.getElementById("pinload"));
var page = document.getElementsByClassName("container")[0];
page.setAttribute("style", "display:block;");
}	
function hideBox(id) {
var x = document.getElementById("TimeDateDiv"+id);
if (x.style.display === "none") {
	x.style.display = "block";
} else {
	x.style.display = "none";
}
}