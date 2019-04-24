	 var set_real_time;
	 
	function real_time(hours,min,sec) {
   sec=Number(sec)+1;
    if (sec>=60){min=Number(min)+1;sec=0;}
    if (min>=60){hours=Number(hours)+1;min=0;}
    if (hours>=24){hours=0};
    document.getElementById("time").innerHTML = hours+":"+min+":"+sec;
    set_real_time = setTimeout("real_time("+hours+","+min+","+sec+");", 1000);
   }
	function handleServerResponse(){
    clearTimeout(set_real_time);
    var res = jsonResponse.time.split(":");
    real_time(hours=res[0],min=res[1],sec=res[2]);
    document.body.style.backgroundColor="rgb("+jsonResponse.rgb+")";
   }
   
   function load_time(submit){
    server = "/Time";
    send_request(submit,server);
    load();
   }
	 
	function set_pinout(submit,val,sw){
	server = "/pins?pin"+val+"="+sw;
    send_request(submit,server);
	switch_pos[val]=sw;
   }
	function set_pin_name(submit,num){
	server = "/pins?pin"+num+"="+val('pin'+num);
    send_request(submit,server);
   }
   
    function set_fadeon(submit,val,sw){
	server = "/mode?fadeon"+val+"="+sw;
    send_request(submit,server);
	fadeon[val]=sw;
	}
   
    function all_off(submit){
	for (var i = 1; i < 8; i++) {
	check(i,0)
	}
	server = "/pins?pin0=0&pin1=0&pin2=0&pin3=0&pin4=0&pin5=0&pin6=0"
	send_request(submit,server);
	}


	function check(pin,sw) {
    if(sw==1){
	document.getElementById("switch-radio-off-"+pin).checked = true;
    document.getElementById("switch-radio-on-"+pin).checked = false;
	}
	if(sw==0){
    document.getElementById("switch-radio-off-"+pin).checked = false;
    document.getElementById("switch-radio-on-"+pin).checked = true;
	}
	}
	function fadecheck(pin,sw){
	if(sw==1){
	document.getElementById("mode"+pin+"st").checked = false;
    document.getElementById("mode"+pin+"fa").checked = true;
	}
	if(sw==0){
    document.getElementById("mode"+pin+"st").checked = true;
    document.getElementById("mode"+pin+"fa").checked = false;
	}
	}

	function sw_load(submit){
	for (var i = 1; i < 8; i++) {
	check(i,switch_pos[i-1]);
	}
    server = "/pins?pin0="+switch_pos[0]+"&pin1="+switch_pos[1]+"&pin2="+switch_pos[2]+"&pin3="+switch_pos[3]+"&pin4="+switch_pos[4]+"&pin5="+switch_pos[5]+"&pin6="+switch_pos[6];
	send_request(submit,server);
	
	for (var i = 1; i < 4; i++) {
	fadecheck(i,fadeon[i-1]);
	}
    server = "/mode?fadeon0="+fadeon[0]+"&fadeon1="+fadeon[1]+"&fadeon2="+fadeon[2];
	send_request(submit,server);
	}
	
	function set_settings(submit,param,vect,ch){
	var t;
	if(param==0) {param='bright'; t = bright[ch];}
	if(param==1) {param='amount'; t = amount[ch];}
	
	if((param=='bright'&&t>0&&t<254)||(param=='amount'&&t>0&&t<29))
	{if(vect==1) t++;}
	if((param=='bright'&&t<255&&t>1)||(param=='amount'&&t<30&&t>1))	
	{if(vect==0) t--;}
	 console.log(param+ch.toString()+'='+t);
	 server = "/mode?"+param+ch.toString()+"="+t;
	send_request(submit,server);
	document.getElementById(param+ch.toString()).innerHTML = t;
	if(param=='bright') bright[ch]=t;
	if(param=='amount') amount[ch]=t;
	}
	