const main = () => {
    
    const gameAreaEl = document.getElementById("gameArea");
    const glCanvas = document.getElementById("gl");
    const ctxCanvas = document.getElementById("cvs");

    ctxCanvas.width = glCanvas.width = parseFloat(getComputedStyle(gameAreaEl).getPropertyValue("width"));
    ctxCanvas.height = glCanvas.height = parseFloat(getComputedStyle(gameAreaEl).getPropertyValue("height"));
    Module._setWindowSize(glCanvas.width, glCanvas.height);
}

addEventListener("load", main);