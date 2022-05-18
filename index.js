const gameBoard = document.querySelector("#gameBoard");
const scoreText = document.querySelector("#scoreText");
const resetButton = document.querySelector("#resetButton");
const ctx = gameBoard.getContext("2d");

const gameWidth = gameBoard.width;
const gameHeight = gameBoard.height;
const block = 25;

const boardBackground = "black";
const snakeColor = "green";
const snakeBorder = "white";
const foodColor = "red";

const UP = 87;
const DOWN = 83;
const LEFT = 65;
const RIGHT = 68;

let running = false;

let snake;

let xVelocity;
let yVelocity;

let xFood;
let yFood;

let score;

window.addEventListener("keydown", changeDirection);
resetButton.addEventListener("click", resetGame);

gameStart();

function gameStart() {

    running = true;
    initVars();
    createFood();
    drawFood();
    nextTick();
};

function initVars () {

    score = 0;
    scoreText.textContent = score
    xVelocity = block;
    yVelocity = 0;
    snake = newSnake();
}

function newSnake() {

    return [
        {x:block * 4, y:0},
        {x:block * 3, y:0},
        {x:block * 2, y:0},
        {x:block * 1, y:0},
        {x:block * 0, y:0}
    ]
}

function nextTick() {

    if(running) {
        setTimeout(()=>{
            clearBoard();
            drawFood();
            moveSnake();
            drawSnake();
            checkGameOver();
            nextTick();
        }, 75)
    } else {
        displayGameOver();
    }
};

function clearBoard() {

    ctx.fillStyle = boardBackground;
    ctx.fillRect(0,0, gameWidth, gameHeight);
};

function createFood(){

    function randomFood(min, max){
        const randNum = Math.round((Math.random() * (max - min) + min) / block) * block;
        return randNum;
    }

    xFood = randomFood(0, gameWidth - block);
    yFood = randomFood(0, gameWidth - block);
};


function drawFood() {

    ctx.fillStyle = foodColor;
    ctx.fillRect(xFood, yFood, block, block)
};

function moveSnake() {

    const head = {
        x:snake[0].x + xVelocity,
        y:snake[0].y + yVelocity
    };

    snake.unshift(head);

    if(snake[0].x == xFood && snake[0].y == yFood) {
        score += 1;
        scoreText.textContent = score;
        createFood();
    } else {
        snake.pop();
    }
};

function changeDirection(event) {

    const keyPressed = event.keyCode;

    const goingUp = (yVelocity == -block);
    const goingDown = (yVelocity == block);
    const goingLeft = (xVelocity == -block);
    const goingRight = (xVelocity == block);

    switch(true) {
        case(keyPressed == LEFT && !goingRight):
            xVelocity = -block;
            yVelocity = 0;
            break;
        case(keyPressed == RIGHT && !goingLeft):
            xVelocity = block;
            yVelocity = 0;
            break;
        case(keyPressed == UP && !goingDown):
            xVelocity = 0;
            yVelocity = -block;
            break;
        case(keyPressed == DOWN && !goingUp):
            xVelocity = 0;
            yVelocity = block;
            break;
    }
};

function drawSnake() {

    ctx.fillStyle = snakeColor;
    ctx.strokeStyle = snakeBorder;

    snake.forEach(snakePart => {
        ctx.fillRect(snakePart.x, snakePart.y, block, block);
        ctx.strokeRect(snakePart.x, snakePart.y, block, block);
    })
};

function checkGameOver() {

    switch(true) {
        case(snake[0].x < 0):
            running = false;
            break;
        case(snake[0].x >= gameWidth):
            running = false;
            break;
        case(snake[0].y < 0):
            running = false;
            break;
        case(snake[0].y >= gameHeight):
            running = false;
            break;        
    }

    for(let i = 1; i < snake.length; i++) {

        if(snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
            running = false;
        }
    }
};

function displayGameOver() {

    ctx.font = "50px MV Boli"
    ctx.fillStyle = "white"
    ctx.textAlign = "center"
    ctx.fillText("GAME OVER", gameWidth/2, gameHeight/2);
    running = false;
};

function resetGame() {
    
    gameStart();
};