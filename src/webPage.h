#include <Arduino.h>

void initWebserver();


extern const char* PARAM_INPUT_1;
extern const char* PARAM_INPUT_2;
extern int setTemp;
extern int tempMin;
extern int tempMax;
extern String fanControll;


const char webpage[] PROGMEM = R"rawliteral(
<html lang="en">

<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,minimum-scale=1">
    <meta name="theme-color" content="#474E65">
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="application-name" content="miniTermo">
    <title>miniTermo</title>
    <link href='https://cdnjs.cloudflare.com/ajax/libs/tailwindcss/2.2.19/tailwind.min.css' rel='stylesheet' type='text/css'>
    <link href='https://fonts.googleapis.com/css?family=Open+Sans:400,700,300,600|Open+Sans+Condensed:300,700' rel='stylesheet' type='text/css'>
    <script type="text/javascript" src="https://unpkg.co/gsap@3/dist/gsap.min.js"></script>
    <script type="text/javascript" src="https://unpkg.com/gsap@3/dist/Draggable.min.js"></script>
    <script src="https://code.jquery.com/jquery-3.6.0.min.js" integrity="sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4=" crossorigin="anonymous"></script>
</head>

<body class="">
    <div id="stop" class=" bg-purple-600 font-semibold text-white p-2 w-32 rounded-full hover:bg-purple-700">Stop</div>
    <div id="start" class="  bg-purple-600 font-semibold text-white p-2 w-32 rounded-full hover:bg-purple-700 focus:outline-none focus:ring shadow-lg hover:shadow-none transition-all duration-300 m-2">Start</div>
    <div class="temp">


        <div class="temp__dial">

            <div class="temp__drag"></div>
            <div class="temp__dial-shades">
                <div class="temp__shade-cold"></div>
                <div class="temp__shade-hot"></div>
            </div>
            <div class="temp__dial-core">
            </div>
            <div class="ring">

                <svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" width="280.5px" height="280.5px" viewBox="0 0 280.5 280.5" style="overflow:visible;enable-background:new 0 0 280.5 280.5;" xml:space="preserve">
                    <style type="text/css">
                        .st0 {
                            fill: #222;
                            opacity: 0.1
                        }

                    </style>
                    <defs>
                    </defs>
                    <rect x="4.2" y="123.1" transform="matrix(-1.836970e-16 1 -1 -1.836970e-16 157.3841 123.0695)" class="st0" width="25.9" height="34.3" />
                    <rect x="250.3" y="123.1" transform="matrix(-1.836970e-16 1 -1 -1.836970e-16 403.5232 -123.0695)" class="st0" width="25.9" height="34.3" />
                    <rect x="127.3" y="246.1" class="st0" width="25.9" height="34.3" />
                    <rect x="127.3" y="0" class="st0" width="25.9" height="34.3" />
                    <rect x="40.2" y="210.1" transform="matrix(0.7071 0.7071 -0.7071 0.7071 176.2731 28.9394)" class="st0" width="25.9" height="34.3" />
                    <rect x="214.3" y="36" transform="matrix(0.7071 0.7071 -0.7071 0.7071 104.1806 -145.1071)" class="st0" width="25.9" height="34.3" />
                    <rect x="214.3" y="210.1" transform="matrix(0.7071 -0.7071 0.7071 0.7071 -94.1301 227.25)" class="st0" width="25.9" height="34.3" />
                    <rect x="40.2" y="36" transform="matrix(0.7071 -0.7071 0.7071 0.7071 -22.0376 53.2035)" class="st0" width="25.9" height="34.3" />
                    <rect x="80.4" y="236.9" transform="matrix(0.9246 0.381 -0.381 0.9246 103.8178 -16.4028)" class="st0" width="25.9" height="34.3" />
                    <rect x="174.1" y="9.3" transform="matrix(0.9246 0.381 -0.381 0.9246 24.1861 -69.2959)" class="st0" width="25.9" height="34.3" />
                    <rect x="241" y="170" transform="matrix(0.381 -0.9246 0.9246 0.381 -15.7669 350.6812)" class="st0" width="25.9" height="34.3" />
                    <rect x="13.5" y="76.2" transform="matrix(0.381 -0.9246 0.9246 0.381 -69.9319 82.2221)" class="st0" width="25.9" height="34.3" />
                    <rect x="174.6" y="236.7" transform="matrix(0.9232 -0.3844 0.3844 0.9232 -83.1631 91.582)" class="st0" width="25.9" height="34.3" />
                    <rect x="80" y="9.5" transform="matrix(0.9232 -0.3844 0.3844 0.9232 -3.0903 37.761)" class="st0" width="25.9" height="34.3" />
                    <rect x="240.9" y="75.8" transform="matrix(-0.3844 -0.9232 0.9232 -0.3844 265.6273 362.9808)" class="st0" width="25.9" height="34.3" />
                    <rect x="13.6" y="170.4" transform="matrix(-0.3844 -0.9232 0.9232 -0.3844 -136.2844 284.1799)" class="st0" width="25.9" height="34.3" />
                </svg>


            </div>

            <div class="temp__value">
                <span class="temp__digit"></span><span class="temp__digit"></span>°
            </div>


        </div>

        <div class="box">
            <div class="temp__outdoors-col">
                <small class="temp__heading">Current</small>
                <br>
                <span class="temp__c-value">0°</span>
            </div>
            <div class="temp__outdoors-col">
                <small class="temp__heading">Outside</small>
                <br>
                <span class="temp__o-value">0°</span>
            </div>
            <div class="temp__outdoors-col">
                <small class="temp__heading">Humidity</small>
                <br>
                <span class="temp__h-value">0%%</span>
            </div>
        </div>

        <div class="input_button">
            <div class="on-off-toggle">

                <input class="on-off-toggle__input" checked type="checkbox" id="bopis" />
                <label for="bopis" class="on-off-toggle__slider"></label>
            </div>
            <p class="labell">wiatrak</p>


        </div>

    </div>


    <style>
        * {
            border: 0;
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        :root {
            --l1: hsl(223, 10%%, 100%%);
            --l2: hsl(223, 10%%, 95%%);
            --l3: hsl(223, 10%%, 90%%);
            --l4: hsl(223, 10%%, 85%%);
            --l5: hsl(223, 10%%, 80%%);
            --l6: hsl(223, 10%%, 75%%);
            --text: hsl(223, 10%%, 20%%);
            font-size: calc(16px + (24 - 16) * (100vw - 320px) / (1280 - 320));
        }

        #start,
        #stop {
            display: none;
        }

        body {
            font-family: OpenSans, sans-serif;
            height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            background: var(--l3);
            color: var(--text);
            display: grid;
            place-items: center;
            font: 1em/1.5 Nunito, sans-serif;
            height: 100vh;
        }

        .temp {
            --angle: 0deg;
            background-color: transparent;
            border-radius: 0em;
            display: flex;
            flex-direction: column;
            justify-content: center;
            margin: 1.5em 0;
            padding: 2em;
            width: 16em;
            height: 28.4em;
        }

        .temp .ring svg {

            width: 100%%;
            height: 100%%;
        }

        .temp .ring {
            overflow: hidden;
            margin: 0;
            width: 100%%;
            height: 100%%;
            opacity: 1.04;
            position: relative;
            box-shadow: none !important;
        }

        .temp__dial,
        .temp__dial-core,
        .temp__dial-shades,
        .temp__shade-cold,
        .temp__shade-hot,
        .temp__drag,
        .temp__drag:before {
            border-radius: 50%%;
        }

        .temp__dial {
            box-shadow:
                0.5em 0.5em 1em var(--l6),
                -0.5em -0.5em 1em var(--l1);
            margin-bottom: 2.5em;
            position: relative;
            width: 12em;
            height: 12em;
        }

        .temp__dial-core,
        .temp__dial-shades,
        .temp__shade-cold,
        .temp__shade-hot,
        .temp__value,
        .temp__drag,
        .temp__drag:before {
            position: absolute;
        }

        .temp__dial-core,
        .temp__dial-shades,
        .temp__value,
        .temp__drag:before {
            top: 50%%;
            left: 50%%;
        }

        .temp__dial-core,
        .temp__dial-shades,
        .temp__value {
            transform: translate(-50%%, -50%%);
        }

        .temp__dial-core,
        .temp__dial-shades,
        .temp__value {
            transition: all 0.2s ease-in-out;
        }

        .temp__dial-core,
        .temp__dial-shades {
            transition-delay: 0.1s;
            width: 8.5rem;
            height: 8.5rem;
        }

        .temp__dial-core {
            box-shadow: 0 0.2rem 0.5rem 0.1rem inset;
            color: hsla(0, 0%%, 0%%, 0.1);
            border: solid 20px rgba(20, 20, 20, 0.3);
        }

        .temp__dial-shades,
        .temp__drag:before {
            opacity: 0;
        }

        .temp__shade-cold,
        .temp__shade-hot,
        .temp__drag {
            width: 100%%;
            height: 100%%;
            opacity: 0.4;
        }

        .temp__shade-cold {
          
          background-image: radial-gradient(100%% 100%% at 50%% 100%%, hsl(193, 90%%, 55%%), hsl(268, 90%%, 55%%)); 
          
          -webkit-background-size: cover;
          -moz-background-size: cover;
          -o-background-size: cover;
          background-size: cover;
          background-attachment: scroll, scroll, scroll, fixed;

            /*  background: rgb(112,47,189);
                background: -moz-radial-gradient(circle, rgba(112,47,189,1) 0%%, rgba(38,199,244,1) 100%%);
                background: -webkit-radial-gradient(circle, rgba(112,47,189,1) 0%%, rgba(38,199,244,1) 100%%);
                background: radial-gradient(circle, rgba(112,47,189,1) 0%%, rgba(38,199,244,1) 100%%);
                filter: progid:DXImageTransform.Microsoft.gradient(startColorstr="#702fbd",endColorstr="#26c7f4",GradientType=1);*/
        }

        .temp__shade-hot {
            background-image: radial-gradient(100%% 100%% at 50%% 100%%, hsl(63, 90%%, 55%%), hsl(13, 90%%, 45%%));

          -webkit-background-size: cover;
          -moz-background-size: cover;
          -o-background-size: cover;
          background-size: cover;
          background-attachment: scroll, scroll, scroll, fixed;
            
        }

        .temp__drag {
            cursor: grab;
            z-index: 1;
        }

        .temp__drag:active,
        .temp__drag--active {
            cursor: grabbing;
        }

        .temp__drag:active~.temp__dial-core,
        .temp__drag--active~.temp__dial-core,
        .temp__drag:active~.temp__dial-shades,
        .temp__drag--active~.temp__dial-shades {
            transition-delay: 0s;
            width: 6em;
            height: 6em;
        }

        .temp__drag:active~.temp__dial-core,
        .temp__drag--active~.temp__dial-core {
            color: hsla(0, 0%%, 0%%, 0.3);
        }

        .temp__drag:before {
            background: var(--l5);
            content: "";
            display: block;
            top: 50%%;
            left: 50%%;
            width: 2em;
            height: 2em;
            transform: translate(-50%%, -50%%) translateY(4.5em) rotate(calc(-1 * var(--angle)));
            transition: opacity 0.2s ease-in-out;
        }

        .temp__drag:active:before,
        .temp__drag--active:before,
        .temp__drag:active~.temp__dial-shades,
        .temp__drag--active~.temp__dial-shades {
            opacity: 1;
        }

        .temp__drag:active:before,
        .temp__drag--active:before {
            transition-delay: 0.1s;
        }

        .temp__drag:active~.temp__value,
        .temp__drag--active~.temp__value {
            color: hsl(223, 10%%, 100%%);
        }

        .temp__value {
            font-size: 2.5em;
            font-weight: bold;
            text-align: right;
            width: 3ch;
        }

        .temp__digit {
            display: inline-block;
        }

        .temp__digit--inc {
            animation: digitA 0.15s linear, digitB 0.15s 0.15s linear;
        }

        .temp__digit--dec {
            animation: digitB 0.15s linear reverse, digitA 0.15s 0.15s linear reverse;
        }

        .temp__heading {
            font-size: 0.4em;
            letter-spacing: 0.2em;
            text-transform: uppercase;
        }

        .box {
            background-color: var(--l4);
            border-radius: 0.75em;
            box-shadow:
                0 0.1em 0.1em var(--l5) inset,
                0 -0.1em 0.1em var(--l2) inset;
            display: flex;
            justify-content: space-between;
            padding: 0.75em 1.75em;
            text-align: center;
        }

        .temp__o-value,
        .temp__h-value {
            font-size: 1em;
        }

        /* Animations */
        @keyframes digitA {
            from {
                opacity: 1;
                transform: translateY(0);
            }

            to {
                opacity: 0;
                transform: translateY(-25%%);
            }
        }

        @keyframes digitB {
            from {
                opacity: 0;
                transform: translateY(25%%);
            }

            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        .input_button {

            border-radius: 18px;
            background-color: var(--l3);
            border: 1px solid var(--l6);
            padding: 10px;
        }

        /** CSS INPUT SWITCH **/
        .on-off-toggle {
            width: 56px;
            height: 24px;
            position: relative;
            display: inline-block;
            float: right;
        }

        .on-off-toggle__slider {
            width: 56px;
            height: 24px;
            display: block;
            border-radius: 34px;
            background-color: #d8d8d8;
            transition: background-color 0.4s
        }

        .on-off-toggle__slider:before {
            content: '';
            display: block;
            background-color: #fff;
            box-shadow: 0 0 0 1px var(--text);
            bottom: 3px;
            height: 18px;
            left: 3px;
            position: absolute;
            transition: .4s;
            width: 18px;
            z-index: 5;
            border-radius: 100%%;
        }

        .on-off-toggle__slider:after {
            display: block;
            line-height: 24px;
            text-transform: uppercase;
            font-size: 12px;
            font-weight: bold;
            content: 'off';
            color: #484848;
            padding-left: 26px;
            transition: all 0.4s;
        }

        .on-off-toggle__input {
            position: absolute;
            opacity: 0;
        }

        .on-off-toggle__input:checked+.on-off-toggle__slider {
            background-color: var(--green)
        }

        .on-off-toggle__input:checked+.on-off-toggle__slider:before {
            transform: translateX(32px);
        }

        .on-off-toggle__input:checked+.on-off-toggle__slider:after {
            content: 'on';
            color: #FFFFFF;
            padding-left: 8px;
        }

        .input_button p {
            float: left;
            font-size: 0.7em;
            opacity: 0.8;
            margin-right: 20px;
            color: var(--text);
        }

        /*** END CSS NEEDED FOR SWITCH **/




        /* Dark theme */
        @media (prefers-color-scheme: dark) {
            :root {
                --l1: hsl(223, 10%%, 50%%);
                --l2: hsl(223, 10%%, 45%%);
                --l3: hsl(223, 10%%, 40%%);
                --l4: hsl(223, 10%%, 35%%);
                --l5: hsl(223, 10%%, 30%%);
                --l6: hsl(223, 10%%, 25%%);
                --green: hsla(78, 100%%, 27%%, 1);
                --text: hsl(223, 10%%, 80%%);
            }
        }

        @media (prefers-color-scheme: light) {
            :root {

                --green: hsla(78, 100%%, 27%%, 1);
            }
        }

    </style>
    <script type="text/javascript">
        var startAnim = gsap.to(".ring", {
            rotation: "+=360",
            ease: "power1.in",
            paused: true,
            duration: 3,
            onComplete: function() {

                startAnim.pause();
                loopAnim.play(0);

            }
        });

        var loopAnim = gsap.to(".ring", {
            rotation: "+=360",
            ease: "none",

            repeat: -1,
            onComplete: function() {
                stateAnim = "play";
            },
            paused: true
        });



        var stopAnim = gsap.to(".ring", {
            rotation: 180,
            duration: 0.6,
            paused: true,
            onComplete: function() {

                loopAnim.pause();
                startAnim.pause();

            }
        });


        $("#start").on('click', function() {


            if (loopAnim.paused() & startAnim.paused()) {

                startAnim.play(0);


            }

        });

        $("#stop").on('click', function() {
            if (!loopAnim.paused() & startAnim.paused() || !startAnim.paused()) {
                stopAnim.play(0);
            }
        });







        document.addEventListener("DOMContentLoaded", function() {



            let thermostat = new NeuThermostat(".temp");
            $.ajax({
                type: "GET",
                url: "https://api.openweathermap.org/data/2.5/weather?lat=49.880870&lon=19.563030&appid=f055d509de51700a688e61d5f8e3da76&units=metric",
                dataType: "json",
                success: function(json) {
                    thermostat.outdoorsAdjust(json.main.temp, json.main.humidity);
                   
                }
            });

             function fetchAjax() {
                    $.ajax({
                        type: "GET",
                        url: "/params",
                        dataType: "json",
                        success: function(datax) {
                            console.log(datax);
                            thermostat.tempAdjust(datax.setTemp);
                            $(".temp__c-value").html(datax.temp + "°")

                            if (datax.fan == "checked") {
                                $(".on-off-toggle__input").prop('checked', true);
                                var lAnimPaused = loopAnim.paused();

                                if (loopAnim.paused() & startAnim.paused()) {

                                    startAnim.play(0);


                                }

                            } else {
                                $(".on-off-toggle__input").prop('checked', false);
                                if (!loopAnim.paused() & startAnim.paused() || !startAnim.paused()) {
                                    stopAnim.play(0);
                                }

                            }

                        }

                    });

                };

                    setInterval(fetchAjax, 1200);



        });

        class NeuThermostat {
            
            constructor(el) {
                
                this.el = document.querySelector(el);
                this.temp = %_setTemp_%;
                this.tempMin = %_tempMin_%;
                this.tempMax = %_tempMax_%;
                this.angleMin = 15;
                this.angleMax = 345;
                this.outside = 0;
                this.humidity = 0;

                this.init();
            }
            
            init() {
                window.addEventListener("keydown", this.kbdEvent.bind(this));
                window.addEventListener("keyup", this.activeState.bind(this));

                // hard limits
                if (this.tempMin < 0)
                    this.tempMin = 0;

                if (this.tempMax > 99)
                    this.tempMax = 99;

                if (this.angleMin < 0)
                    this.angleMin = 0;

                if (this.angleMax > 360)
                    this.angleMax = 360;

                // init values
                this.tempAdjust(this.temp);
                this.outdoorsAdjust(this.outside, this.humidity);
                    this.getData();
                // init GreenSock Draggable
                Draggable.create(".temp__drag", {
                    type: "rotation",
                    bounds: {
                        minRotation: this.angleMin,
                        maxRotation: this.angleMax
                    },
                    onDrag: () => {

                        $(".ring").fadeOut("slow");
                        this.tempAdjust("drag");

                    },
                    onDragEnd: function() {
                        // send value to arduino
                        $(".ring").fadeIn("slow");

                        $.ajax({
                            "url": "/setTemp?setTemp=" + actualTemp.toFixed(0)
                        });

                    }
                });
            }
            angleFromMatrix(transVal) {
                let matrixVal = transVal.split('(')[1].split(')')[0].split(','),
                    [cos1, sin] = matrixVal.slice(0, 2),
                    angle = Math.round(Math.atan2(sin, cos1) * (180 / Math.PI)) * -1;

                // convert negative angles to positive
                if (angle < 0)
                    angle += 360;

                if (angle > 0)
                    angle = 360 - angle;

                return angle;
            }
            randInt(min, max) {
                return Math.round(Math.random() * (max - min)) + min;
            }
            kbdEvent(e) {
                let kc = e.keyCode;

                if (kc) {
                    // up or right
                    if (kc == 38 || kc == 39)
                        this.tempAdjust("u");

                    // left or down
                    else if (kc == 37 || kc == 40)
                        this.tempAdjust("d");
                }
            }
            activeState(shouldAdd = false) {


                if (this.el) {
                    let dragClass = "temp__drag",
                        activeState = dragClass + "--active",
                        tempDrag = this.el.querySelector("." + dragClass);

                    if (tempDrag) {
                        if (shouldAdd === true)
                            tempDrag.classList.add(activeState);
                        else
                            tempDrag.classList.remove(activeState);
                    }
                }
            }
            removeClass(el, classname) {
                el.classList.remove(classname);
            }
            changeDigit(el, digit) {
                el.textContent = digit;
            }
            tempAdjust(inputVal = 0) {
                /*
                inputVal can be the temp as an integer, "u" for up, 
                "d" for down, or "drag" for dragged value
                */
                if (this.el) {
                    let cs = window.getComputedStyle(this.el),
                        tempDigitEls = this.el.querySelectorAll(".temp__digit"),
                        tempDigits = tempDigitEls ? Array.from(tempDigitEls).reverse() : [],
                        tempDrag = this.el.querySelector(".temp__drag"),
                        cold = this.el.querySelector(".temp__shade-cold"),
                        hot = this.el.querySelector(".temp__shade-hot"),
                        prevTemp = Math.round(this.temp),
                        tempRange = this.tempMax - this.tempMin,
                        angleRange = this.angleMax - this.angleMin,
                        notDragged = inputVal != "drag";

                    // input is an integer
                    if (!isNaN(inputVal)) {
                        this.temp = inputVal;

                        // input is a given direction
                    } else if (inputVal == "u") {
                        if (this.temp < this.tempMax)
                            ++this.temp;

                        this.activeState(true);

                    } else if (inputVal == "d") {
                        if (this.temp > this.tempMin)
                            --this.temp;

                        this.activeState(true);

                        // Draggable was used
                    } else if (inputVal == "drag") {
                        if (tempDrag) {
                            let tempDragCS = window.getComputedStyle(tempDrag),
                                trans = tempDragCS.getPropertyValue("transform"),
                                dragAngle = this.angleFromMatrix(trans),
                                relAngle = dragAngle - this.angleMin,
                                angleFrac = relAngle / angleRange;

                            this.temp = angleFrac * tempRange + this.tempMin;
                            actualTemp = this.temp;


                        }
                    }

                    // keep the temperature within bounds
                    if (this.temp < this.tempMin)
                        this.temp = this.tempMin;
                    else if (this.temp > this.tempMax)
                        this.temp = this.tempMax;

                    // use whole number temperatures for keyboard control
                    if (notDragged)
                        this.temp = Math.round(this.temp);

                    let relTemp = this.temp - this.tempMin,
                        tempFrac = relTemp / tempRange,
                        angle = tempFrac * angleRange + this.angleMin;

                    // CSS variable
                    this.el.style.setProperty("--angle", angle + 'deg');
                    // draggable area
                    if (tempDrag && notDragged)
                        tempDrag.style.transform = `rotate(angle+"deg")`;

                    // shades
                    if (cold)
                        cold.style.opacity = 1 - tempFrac;
                    if (hot)
                        hot.style.opacity = tempFrac;

                    // display value
                    if (tempDigits) {
                        let prevDigitArr = String(prevTemp).split("").reverse(),
                            tempRounded = Math.round(this.temp),
                            digitArr = String(tempRounded).split("").reverse(),
                            maxDigits = 2,
                            digitDiff = maxDigits - digitArr.length,
                            prevDigitDiff = maxDigits - prevDigitArr.length,
                            incClass = "temp__digit--inc",
                            decClass = "temp__digit--dec",
                            timeoutA = 150,
                            timeoutB = 300;

                        while (digitDiff--)
                            digitArr.push("");

                        while (prevDigitDiff--)
                            prevDigitArr.push("");

                        for (let d = 0; d < maxDigits; ++d) {
                            let digit = +digitArr[d],
                                prevDigit = +prevDigitArr[d],
                                tempDigit = tempDigits[d];

                            setTimeout(this.changeDigit.bind(null, tempDigit, digit), timeoutA);

                            // animate increment
                            if ((digit === 0 && prevDigit === 9) || (digit > prevDigit && this.temp > prevTemp)) {
                                this.removeClass(tempDigit, incClass);
                                void tempDigit.offsetWidth;
                                tempDigit.classList.add(incClass);
                                setTimeout(this.removeClass.bind(null, tempDigit, incClass), timeoutB);

                                // animate decrement
                            } else if ((digit === 9 && prevDigit === 0) || (digit < prevDigit && this.temp < prevTemp)) {
                                this.removeClass(tempDigit, decClass);
                                void tempDigit.offsetWidth;
                                tempDigit.classList.add(decClass);
                                setTimeout(this.removeClass.bind(null, tempDigit, decClass), timeoutB);
                            }
                        }
                    }
                }
            }
            outdoorsAdjust(inputOutside = 0, inputHumidity = 0) {
                let outdoorEls = this.el.querySelectorAll(".temp__o-value,.temp__h-value "),
                    outdoorVals = outdoorEls ? Array.from(outdoorEls) : [];

                this.outside = inputOutside;
                this.humidity = inputHumidity;

                if (outdoorVals) {
                    outdoorVals[0].textContent = this.outside + "°";
                    outdoorVals[1].textContent = this.humidity + "%%";
                }
            }
            getData() {

                

               


            }
           
        }let actualTemp = 0;

    </script>

</body>

</html>


)rawliteral";
