(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([["app"],{0:function(e,t,n){e.exports=n("2f39")},"2f39":function(e,t,n){"use strict";n.r(t);var a={};n.r(a),n.d(a,"latestSensorData",function(){return V}),n.d(a,"yesterdaySensorData",function(){return P}),n.d(a,"lastYearSensorData",function(){return R}),n.d(a,"pVoltage",function(){return _}),n.d(a,"pMoisture",function(){return E});var r={};n.r(r),n.d(r,"setSensorReadings",function(){return q}),n.d(r,"addOlderSensorReadings",function(){return N}),n.d(r,"addSensorReadings",function(){return U});var s={};n.r(s),n.d(s,"loadNewSensorData",function(){return J}),n.d(s,"loadInitialSensorData",function(){return B});var o=n("967e"),c=n.n(o),u=(n("96cf"),n("fa84")),i=n.n(u),d=(n("35fc"),n("7d6e"),n("62f2"),n("7e6d"),n("a026")),f=n("8686"),b=n("b178");d["a"].use(b["b"],{config:{},iconSet:f["a"]});var j=function(){var e=this,t=e.$createElement,n=e._self._c||t;return n("div",{attrs:{id:"q-app"}},[n("router-view")],1)},l=[],p={name:"App",data:function(){return{interval:null,versionString:2}},mounted:function(){var e=this;this.$store.dispatch("sensor/loadInitialSensorData").then(function(){e.interval=setInterval(function(){e.periodicUpdate()},1e4)})},methods:{periodicUpdate:function(){this.$store.dispatch("sensor/loadNewSensorData").then(function(){})}},beforeDestroy:function(){console.log("Clearing"),clearInterval(this.interval)}},h=p,m=n("2877"),v=Object(m["a"])(h,j,l,!1,null,null,null),g=v.exports,y=n("2f62"),k=(n("8e6e"),n("8a81"),n("ac6a"),n("cadf"),n("06db"),n("456d"),n("c47a")),w=n.n(k);function D(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);t&&(a=a.filter(function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable})),n.push.apply(n,a)}return n}function z(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?D(n,!0).forEach(function(t){w()(e,t,n[t])}):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):D(n).forEach(function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))})}return e}var O={minVoltage:13,maxVoltage:260,minMoisture:10,maxMoisture:460},x=function(){return z({},O,{empty:{date:void 0,temperature:0,humidity:0,voltage:O.minVoltage,brightness:0,moisture:O.maxMoisture,moisture_cached:!0,brightness_mt:0,id:0},sensorData:{data:[],date:new Date},latestID:0})},S=(n("7514"),n("c1df")),I=n.n(S),V=function(e){return e.sensorData.data&&e.sensorData.data.length>0?e.sensorData.data[0]:e.empty};function M(e,t,n){return n.sensorData.data&&n.sensorData.data.length>0?n.sensorData.data.find(function(n){return I()().subtract(e,t).isAfter(I()(n.date))}):n.empty}var P=function(e){return M(1,"day",e)},R=function(e){return M(1,"year",e)},_=function(e){return function(t){return Math.round(100*(t-e.minVoltage)/(e.maxVoltage-e.minVoltage))}},E=function(e){return function(t){return 100-Math.round(100*(t-e.minMoisture)/(e.maxMoisture-e.minMoisture))}},$=n("7c52"),q=function(e,t){e.sensorData=t,$["a"].set(e.sensorData,"data",t.data.reverse()),e.latestID=e.sensorData.data[0].id},N=function(e,t){var n=e.sensorData.data.concat(t.data.reverse());$["a"].set(e.sensorData,"data",n),e.latestID=n[0].id},U=function(e,t){var n=t.data.reverse().concat(e.sensorData.data);$["a"].set(e.sensorData,"data",n),e.latestID=n[0].id},C=n("bc3a"),A=n.n(C),G="/api/001";console.log("2.0.3");var J=function(e){var t=e.commit,n=e.state;n.latestID>0&&A.a.get(G+"/data/json/afterID/"+n.latestID).then(function(e){return e.data}).then(function(e){e.data&&e.data.length>0&&t("addSensorReadings",e)})},B=function(e){var t=e.commit;A.a.get(G+"/data/json/latest/600").then(function(e){return e.data}).then(function(e){t("setSensorReadings",e);var n=I()().subtract(1,"year").subtract(1,"day").format(),a=I()().subtract(1,"year").format();A.a.get(G+"/data/json/after/"+n+"/before/"+a).then(function(e){return e.data}).then(function(e){t("addOlderSensorReadings",e)})})},F={namespaced:!0,getters:a,mutations:r,actions:s,state:x};d["a"].use(y["a"]);var L=function(){var e=new y["a"].Store({modules:{sensor:F},strict:!1});return e},Q=n("8c4f"),T=[{path:"/",component:function(){return n.e("2d22c0ff").then(n.bind(null,"f241"))},children:[{path:"",component:function(){return n.e("55992a2e").then(n.bind(null,"8b24"))}}]},{path:"/raw",component:function(){return n.e("2d22c0ff").then(n.bind(null,"f241"))},children:[{path:"",component:function(){return n.e("1caa0ab0").then(n.bind(null,"a2a3"))}}]},{path:"/graphs",component:function(){return n.e("2d22c0ff").then(n.bind(null,"f241"))},children:[{path:"",component:function(){return n.e("150ed2c6").then(n.bind(null,"4cbf"))}}]}];T.push({path:"*",component:function(){return n.e("4b47640d").then(n.bind(null,"e51e"))}});var Y=T;d["a"].use(Q["a"]);var H=function(){var e=new Q["a"]({scrollBehavior:function(){return{x:0,y:0}},routes:Y,mode:"history",base:"/"});return e},K=function(){var e="function"===typeof L?L({Vue:d["a"]}):L,t="function"===typeof H?H({Vue:d["a"],store:e}):H;e.$router=t;var n={el:"#q-app",router:t,store:e,render:function(e){return e(g)}};return{app:n,store:e,router:t}},W=function(){var e=i()(c.a.mark(function e(t){var n;return c.a.wrap(function(e){while(1)switch(e.prev=e.next){case 0:n=t.Vue,n.prototype.$axios=A.a;case 2:case"end":return e.stop()}},e)}));return function(t){return e.apply(this,arguments)}}(),X=K(),Z=X.app,ee=X.store,te=X.router;function ne(){return ae.apply(this,arguments)}function ae(){return ae=i()(c.a.mark(function e(){var t,n;return c.a.wrap(function(e){while(1)switch(e.prev=e.next){case 0:t=[W],n=0;case 2:if(!(n<t.length)){e.next=20;break}if("function"===typeof t[n]){e.next=5;break}return e.abrupt("continue",17);case 5:return e.prev=5,e.next=8,t[n]({app:Z,router:te,store:ee,Vue:d["a"],ssrContext:null});case 8:e.next=17;break;case 10:if(e.prev=10,e.t0=e["catch"](5),!e.t0||!e.t0.url){e.next=15;break}return window.location.href=e.t0.url,e.abrupt("return");case 15:return console.error("[Quasar] boot error:",e.t0),e.abrupt("return");case 17:n++,e.next=2;break;case 20:new d["a"](Z);case 21:case"end":return e.stop()}},e,null,[[5,10]])})),ae.apply(this,arguments)}ne()},4678:function(e,t,n){var a={"./af":"2bfb","./af.js":"2bfb","./ar":"8e73","./ar-dz":"a356","./ar-dz.js":"a356","./ar-kw":"423e","./ar-kw.js":"423e","./ar-ly":"1cfd","./ar-ly.js":"1cfd","./ar-ma":"0a84","./ar-ma.js":"0a84","./ar-sa":"8230","./ar-sa.js":"8230","./ar-tn":"6d83","./ar-tn.js":"6d83","./ar.js":"8e73","./az":"485c","./az.js":"485c","./be":"1fc1","./be.js":"1fc1","./bg":"84aa","./bg.js":"84aa","./bm":"a7fa","./bm.js":"a7fa","./bn":"9043","./bn.js":"9043","./bo":"d26a","./bo.js":"d26a","./br":"6887","./br.js":"6887","./bs":"2554","./bs.js":"2554","./ca":"d716","./ca.js":"d716","./cs":"3c0d","./cs.js":"3c0d","./cv":"03ec","./cv.js":"03ec","./cy":"9797","./cy.js":"9797","./da":"0f14","./da.js":"0f14","./de":"b469","./de-at":"b3eb","./de-at.js":"b3eb","./de-ch":"bb71","./de-ch.js":"bb71","./de.js":"b469","./dv":"598a","./dv.js":"598a","./el":"8d47","./el.js":"8d47","./en-SG":"cdab","./en-SG.js":"cdab","./en-au":"0e6b","./en-au.js":"0e6b","./en-ca":"3886","./en-ca.js":"3886","./en-gb":"39a6","./en-gb.js":"39a6","./en-ie":"e1d3","./en-ie.js":"e1d3","./en-il":"7333","./en-il.js":"7333","./en-nz":"6f50","./en-nz.js":"6f50","./eo":"65db","./eo.js":"65db","./es":"898b","./es-do":"0a3c","./es-do.js":"0a3c","./es-us":"55c9","./es-us.js":"55c9","./es.js":"898b","./et":"ec18","./et.js":"ec18","./eu":"0ff2","./eu.js":"0ff2","./fa":"8df4","./fa.js":"8df4","./fi":"81e9","./fi.js":"81e9","./fo":"0721","./fo.js":"0721","./fr":"9f26","./fr-ca":"d9f8","./fr-ca.js":"d9f8","./fr-ch":"0e49","./fr-ch.js":"0e49","./fr.js":"9f26","./fy":"7118","./fy.js":"7118","./ga":"5120","./ga.js":"5120","./gd":"f6b4","./gd.js":"f6b4","./gl":"8840","./gl.js":"8840","./gom-latn":"0caa","./gom-latn.js":"0caa","./gu":"e0c5","./gu.js":"e0c5","./he":"c7aa","./he.js":"c7aa","./hi":"dc4d","./hi.js":"dc4d","./hr":"4ba9","./hr.js":"4ba9","./hu":"5b14","./hu.js":"5b14","./hy-am":"d6b6","./hy-am.js":"d6b6","./id":"5038","./id.js":"5038","./is":"0558","./is.js":"0558","./it":"6e98","./it-ch":"6f12","./it-ch.js":"6f12","./it.js":"6e98","./ja":"079e","./ja.js":"079e","./jv":"b540","./jv.js":"b540","./ka":"201b","./ka.js":"201b","./kk":"6d79","./kk.js":"6d79","./km":"e81d","./km.js":"e81d","./kn":"3e92","./kn.js":"3e92","./ko":"22f8","./ko.js":"22f8","./ku":"2421","./ku.js":"2421","./ky":"9609","./ky.js":"9609","./lb":"440c","./lb.js":"440c","./lo":"b29d","./lo.js":"b29d","./lt":"26f9","./lt.js":"26f9","./lv":"b97c","./lv.js":"b97c","./me":"293c","./me.js":"293c","./mi":"688b","./mi.js":"688b","./mk":"6909","./mk.js":"6909","./ml":"02fb","./ml.js":"02fb","./mn":"958b","./mn.js":"958b","./mr":"39bd","./mr.js":"39bd","./ms":"ebe4","./ms-my":"6403","./ms-my.js":"6403","./ms.js":"ebe4","./mt":"1b45","./mt.js":"1b45","./my":"8689","./my.js":"8689","./nb":"6ce3","./nb.js":"6ce3","./ne":"3a39","./ne.js":"3a39","./nl":"facd","./nl-be":"db29","./nl-be.js":"db29","./nl.js":"facd","./nn":"b84c","./nn.js":"b84c","./pa-in":"f3ff","./pa-in.js":"f3ff","./pl":"8d57","./pl.js":"8d57","./pt":"f260","./pt-br":"d2d4","./pt-br.js":"d2d4","./pt.js":"f260","./ro":"972c","./ro.js":"972c","./ru":"957c","./ru.js":"957c","./sd":"6784","./sd.js":"6784","./se":"ffff","./se.js":"ffff","./si":"eda5","./si.js":"eda5","./sk":"7be6","./sk.js":"7be6","./sl":"8155","./sl.js":"8155","./sq":"c8f3","./sq.js":"c8f3","./sr":"cf1e","./sr-cyrl":"13e9","./sr-cyrl.js":"13e9","./sr.js":"cf1e","./ss":"52bd","./ss.js":"52bd","./sv":"5fbd","./sv.js":"5fbd","./sw":"74dc","./sw.js":"74dc","./ta":"3de5","./ta.js":"3de5","./te":"5cbb","./te.js":"5cbb","./tet":"576c","./tet.js":"576c","./tg":"3b1b","./tg.js":"3b1b","./th":"10e8","./th.js":"10e8","./tl-ph":"0f38","./tl-ph.js":"0f38","./tlh":"cf75","./tlh.js":"cf75","./tr":"0e81","./tr.js":"0e81","./tzl":"cf51","./tzl.js":"cf51","./tzm":"c109","./tzm-latn":"b53d","./tzm-latn.js":"b53d","./tzm.js":"c109","./ug-cn":"6117","./ug-cn.js":"6117","./uk":"ada2","./uk.js":"ada2","./ur":"5294","./ur.js":"5294","./uz":"2e8c","./uz-latn":"010e","./uz-latn.js":"010e","./uz.js":"2e8c","./vi":"2921","./vi.js":"2921","./x-pseudo":"fd7e","./x-pseudo.js":"fd7e","./yo":"7f33","./yo.js":"7f33","./zh-cn":"5c3a","./zh-cn.js":"5c3a","./zh-hk":"49ab","./zh-hk.js":"49ab","./zh-tw":"90ea","./zh-tw.js":"90ea"};function r(e){var t=s(e);return n(t)}function s(e){if(!n.o(a,e)){var t=new Error("Cannot find module '"+e+"'");throw t.code="MODULE_NOT_FOUND",t}return a[e]}r.keys=function(){return Object.keys(a)},r.resolve=s,e.exports=r,r.id="4678"},"7e6d":function(e,t,n){}},[[0,"runtime","vendor"]]]);