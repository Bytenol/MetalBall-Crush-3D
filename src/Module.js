// import * as dat from "dat.gui"


let moduleInitialized = async() => new Promise((resolve, reject) => {

    Module.onRuntimeInitialized = () => {
        resolve(true);
    }

});

const cameraPos = {
    x: 0,
    y: 0,
    z: 0
};

const initGui = async() => {
    const gui = new dat.GUI();
    const state = { points: 5 };
    gui.add(state, "points");
    gui.add(cameraPos, "x", -5, 5, 0.001);
    gui.add(cameraPos, "y", -5, 5, 0.001);
    gui.add(cameraPos, "z", -5, 5, 0.001);

}

const main = async() => {
    
    const gameAreaEl = document.getElementById("gameArea");
    const glCanvas = document.getElementById("gl");
    const ctxCanvas = document.getElementById("cvs");

    ctxCanvas.width = glCanvas.width = parseFloat(getComputedStyle(gameAreaEl).getPropertyValue("width"));
    ctxCanvas.height = glCanvas.height = parseFloat(getComputedStyle(gameAreaEl).getPropertyValue("height"));

    // initGui();
    await moduleInitialized();
    Module._setWindowSize(glCanvas.width, glCanvas.height);

    // const g = () => {
    //     Module._setCameraPos(cameraPos.x, cameraPos.y, cameraPos.z);
    //     requestAnimationFrame(g);
    // }
    // requestAnimationFrame(g);
}

addEventListener("load", main);