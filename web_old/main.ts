/**
 * make ball bounce proportional to rotation size
 * make mesh disapper
 */
import * as glm from "./src/esm/index.js";
import {Shader} from "./src/shader.js";
import {createVBO} from "./src/arrayBuffer.js";
import {Mesh} from "./src/mesh.js";
import {ObjParser} from "./src/objParser.js";

let gl: WebGLRenderingContext;
let ctx: CanvasRenderingContext2D;
let staticMesh: Mesh;
let ballMesh: Mesh;
let dynamicMesh: Mesh;
const shaders: Shader[] = [];

const meshData: any = { };

const gravity = 10;

let evt = {
    pointerHeld: false,
}

const ball = {
    velY: 0,
    scale: 0,
}

const randRange = (min: number, max: number) => Math.random() * (max - min + 1) + min;

class Polygon2D {
    #vertices = [];
    #position = [ctx.canvas.width/2, ctx.canvas.height];
    #velocity = [randRange(-30, 35), randRange(-30, -20)];
    #rotation = 0;
    #rotationSpeed = 2;
    #scale = randRange(ctx.canvas.width * 0.03, ctx.canvas.width * 0.04);
    constructor() {
        const steps = [30, 40, 50, 60, 70, 80, 90];
        const sz = 10;
        const stride = steps[~~(Math.random() * steps.length)];
        for (let i = 0; i < 360; i += stride) {
            const ang = glm.glMatrix.toRadian(i);
            this.#vertices.push(Math.cos(ang) * this.#scale);
            this.#vertices.push(Math.sin(ang) * this.#scale);
        }
        Polygon2D.#all.push(this);
    }
    update(dt) {
        this.#rotation += this.#rotationSpeed * dt;
        this.#position[0] += this.#velocity[0] * dt;
        this.#position[1] += this.#velocity[1] * dt;
        if(this.#position[0] < 0 || this.#position[0] > ctx.canvas.width || this.#position[1] < 0) {
            Polygon2D.#all.splice(Polygon2D.#all.indexOf(this), 1);

        }
    }
    draw() {
        ctx.fillStyle = "rgba(255, 255, 255, 0.2)";
        ctx.save();
        ctx.translate(this.#position[0], this.#position[1]);
        ctx.rotate(this.#rotation);
        for (let i = 0; i < this.#vertices.length; i += 2) {
            const x = this.#vertices[i];
            const y = this.#vertices[i + 1];
            if (i === 0) {
                ctx.beginPath();
                ctx.moveTo(x, y);
            }
            else {
                ctx.lineTo(x, y);
            }
        }
        ctx.closePath();
        ctx.fill();
        ctx.restore();
    }

    static spawnTime = 0.4;
    static spawnTimeCounter = 0;

    static #all = [];
    static reset() {
        this.#all = [];
        this.spawnTimeCounter = 0;
    }
    static getAll() { return Polygon2D.#all; }
}


/**
 * This is the principal class for the obstacle's object
 */
class Obstacle {

    /**
     * @constructor
     * @param origin
     * @param pos Position of the obstalce
     * @param vertices is the geometry data
     * @param uvs is the color/texture data
     * @param normals is the normals data
     * @param isPath state if the obstacle is a wall or not
     */
    constructor(public origin: Array<Array<number>>, public pos: number[], public vertices: number[],
                public uvs: number[], public normals: number[], public isPath: boolean) {

    }


    /**
     * This is the buffer size for all buffer data of this object,
     * - vertices, uvs, normals
     */
    static getSize(): number {
        return (54 * 3) * Float32Array.BYTES_PER_ELEMENT
    }


    static reset() {
        this.#all = [];
    }


    static getAll(): Array<Obstacle> {
        return this.#all;
    }


    static #all = [];


    /**
     * This method generates many obstacles arranged in a cicle
     * @param stride is how much the angle should increment
     * @param scale is the scale of the pole's cynlinder, esp the circumference
     * @param yTrans is the translation of the obstacles on the y axis
     */
    static generateGeometryData (stride: number, scale: number, yTrans: number) {
        const maxK = 360 / stride;
        let hasPath = false;
        for (let i = 0, k = 0; i < 360; i += stride, k++) {
            const a1 = glm.glMatrix.toRadian(i);
            const a2 = glm.glMatrix.toRadian(i + stride);

            const rat1 = [Math.cos(a1), Math.sin(a1)];
            const rat2 = [Math.cos(a2), Math.sin(a2)];

            const r = [scale * 2.8, scale * 6];
            const y = yTrans;
            const sh = -(Math.abs(y) + pole.pieceHeight);

            // top face
            const p0 = [rat1[0] * r[0], y, rat1[1] * r[0]];
            const p1 = [rat1[0] * r[1], y, rat1[1] * r[1]];
            const p2 = [rat2[0] * r[1], y, rat2[1] * r[1]];
            const p3 = [rat2[0] * r[0], y, rat2[1] * r[0]];

            const origin = [ glm.vec3.fromValues(...p1), glm.vec3.fromValues(...p2) ];

            // outer face
            const p4 = [p1[0], sh, p1[2]];
            const p5 = [p2[0], sh, p2[2]];

            // inner face
            const p6 = [p0[0], sh, p0[2]];
            const p7 = [p3[0], sh, p3[2]];

            const pos = [0, y, 0];
            const vertices = [
                ...p0, ...p3, ...p2, ...p0, ...p2, ...p1,
                ...p1, ...p2, ...p5, ...p1, ...p5, ...p4,
                ...p3, ...p0, ...p6, ...p3, ...p6, ...p7
            ];

            const normals = [
                ...repeatArray([0, 1, 0], vertices.length/3),
                ...repeatArray([0, 0, 1], vertices.length/3),
                ...repeatArray([0, 0, -1], vertices.length/3),
            ];

            let isPath = Math.random() * 10 > 5;
            if(isPath) hasPath = true;
            if(!hasPath && k === maxK) isPath = true;
            const color = isPath ? [0, 1, 0] : [0, 0, 0];
            let colors = repeatArray(color, vertices.length);
            const obs = new Obstacle(origin, pos, vertices, colors, normals, isPath);
            Obstacle.#all.push(obs);
        }
    }

}

const pole = {
    pieceHeight: 0.15,
    length: -Infinity,
    rotation: 0,
    rotationSpeed: 2,
    angDisplacement: 0,
    angVelocity: 0,
    get height() { return this.pieceHeight * this.length },
};


const GameManager = (() => {
    const generatePoleBottomVertexData = (stride: number, scale: number, yTrans: number, topColor: number[]) => {
        let norm, col;
        let vertices = [];
        let colors = [];
        let normals = [];
        for(let i = 0, k = 0; i < 360; i += stride, k++) {
            let a = glm.glMatrix.toRadian(i);
            let a2 = glm.glMatrix.toRadian(i + stride);
            let r = scale * 4;
            let y = -yTrans;
            let sy = -(yTrans + 0.3);
            let p1 = [0, y, 0];
            let p2 = [Math.cos(a) * r, y, Math.sin(a) * r];
            let p3 = [Math.cos(a2) * r, y, Math.sin(a2) * r];

            let px = Math.cos(a) * r;
            let pz = Math.sin(a) * r;

            let px2 = Math.cos(a2) * r;
            let pz2 = Math.sin(a2) * r;

            vertices.push(...p1, ...p3, ...p2);
            norm = repeatArray([0, 1, 0], 9);
            col = repeatArray(topColor, 9);
            normals.push(...norm);
            colors.push(...col);

            vertices.push(px2, y, pz2, px2, sy, pz2, px, y, pz,px, sy, pz, px, y, pz, px2, sy, pz2);

            norm = repeatArray([0, 0, 1], 18);
            normals.push(...norm);
            col = repeatArray(k % 2 ? [0, 0, 0] : [1, 1, 1], 18);
            colors.push(...col);
        }
        return { vertices, normals, colors };
    }


    const initPoleVertexData = async (url: string) => {
        let txt = await fetchTextFile(url);
        meshData["pole"] = new ObjParser(txt);
        const vertices = [];
        for(let i = 0; i < meshData.pole.vertices.length; i += 3) {
            const x = meshData.pole.vertices[i];
            const y = Math.min(meshData.pole.vertices[i + 1], 0) * pole.pieceHeight;
            const z = meshData.pole.vertices[i + 2];
            vertices.push(x, y, z);
        }

        meshData.pole.vertices = vertices;
        meshData.pole.uvs = repeatArray([1, 1, 1], vertices.length);

        return Promise.resolve(1);
    }


    const init = async () => {
        let txt;
        await initPoleVertexData("./assets/pole.obj");
        txt = await fetchTextFile("./assets/ball.obj");
        meshData["ball"] = new ObjParser(txt);
        dynamicMesh = new Mesh(gl, null, null, null, Obstacle.getSize() * 150, false);
    }


    const startNewLevel = () => {
        pole.length = 50;

        // set the pole and it's bottom mesh
        let scale = 0.3;
        let transformedVertices = transformVertices(meshData.pole.vertices, [0, 0, 0], [scale/2, pole.length, scale/2]);
        const poleBottom = generatePoleBottomVertexData(40, scale, pole.height, [229/255, 189/255, 64/255]);

        let vertices = [...transformedVertices, ...poleBottom.vertices];
        let colors = [...meshData.pole.uvs, ...poleBottom.colors];
        let normals = [...meshData.pole.normals, ...poleBottom.normals];

        staticMesh = new Mesh(gl, vertices, colors, normals);

        // set the ball position
        scale *= 0.5;
        ball.scale = scale * 0.5;
        transformedVertices = transformVertices(meshData.ball.vertices, glm.vec3.fromValues(0, 0, 0), glm.vec3.fromValues(ball.scale, ball.scale, ball.scale));
        let color = repeatArray([237/255, 192/255, 2/255], transformedVertices.length);
        ballMesh = new Mesh(gl, transformedVertices, color, meshData.ball.normals);
        ballMesh.translation = glm.vec3.fromValues(0, 0, scale * 4);

        Obstacle.reset();
        for(let i = -(pole.height - scale); i < 0; i += pole.pieceHeight) {
            Obstacle.generateGeometryData(40, scale, i);
        }
    }

    return { init, startNewLevel };

})();

const update = (dt: number) => {
    // update 2d polygons
    Polygon2D.spawnTimeCounter += dt;
    if(Polygon2D.spawnTimeCounter >= Polygon2D.spawnTime) {
        Polygon2D.spawnTimeCounter = 0;
        new Polygon2D();
    }

    pole.angVelocity = 2 * Math.PI / 2;
    pole.angDisplacement += pole.angVelocity * dt;

    staticMesh.rotation = pole.angDisplacement;
    staticMesh.update(dt);

    // update dynamic mesh
    let minY = Infinity;
    let maxY = -Infinity;
    dynamicMesh.rotation = pole.angDisplacement;
    dynamicMesh.vertices = [];
    dynamicMesh.uvs = [];
    dynamicMesh.normals = [];
    Obstacle.getAll().forEach((obs, i) => {
        minY = Math.min(minY, obs.pos[1]);
        maxY = Math.max(maxY, obs.pos[1]);
        dynamicMesh.vertices.push(...obs.vertices);
        dynamicMesh.uvs.push(...obs.uvs);
        dynamicMesh.normals.push(...obs.normals);
    });
    gl.bindBuffer(gl.ARRAY_BUFFER, dynamicMesh.vbo);
    gl.bufferSubData(gl.ARRAY_BUFFER, 0, new Float32Array([...dynamicMesh.vertices, ...dynamicMesh.uvs, ...dynamicMesh.normals]));
    ballMesh.translation[1] = maxY + Math.abs(Math.sin(staticMesh.rotation*3));

    // collision detection
    const obsCollision = Obstacle.getAll().filter(i => i.pos[1] >= maxY);
    let canMove = false;
    if (ballMesh.translation[1] <= maxY) {
        ballMesh.translation[1] = maxY + ball.scale;

        for(let i = 0; i < obsCollision.length; i++) {
            const obs = obsCollision[i];
            let p0 = glm.vec3.transformMat4(glm.vec3.create(), obs.origin[0], dynamicMesh.modelMatrix);
            let p1 = glm.vec3.transformMat4(glm.vec3.create(), obs.origin[1], dynamicMesh.modelMatrix);
            canMove = p1[0] <= 0 && p0[0] > 0 && obs.isPath;
            if(canMove) break;
        }
    }

    if(evt.pointerHeld && canMove) {
        console.log(1);
        dynamicMesh.translation[1] += 0.1;
        obsCollision.forEach((obs, i) => Obstacle.getAll().splice(Obstacle.getAll().indexOf(obs), 1));
    }

    dynamicMesh.update(dt);
    ballMesh.update(dt);
    // ball.velY -= 0.2 * dt;

    Polygon2D.getAll().forEach((polygon, i) => polygon.update(dt));
}


const draw = () => {
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    shaders[0].use();
    let projectionMatrix = glm.mat4.perspective(glm.mat4.create(), glm.glMatrix.toRadian(45), gl.canvas.width/gl.canvas.height, 0.1, 50);
    let viewMatrix = glm.mat4.lookAt(glm.mat4.create(), glm.vec3.fromValues(0, 1, 5), glm.vec3.fromValues(0, 0, 0), glm.vec3.fromValues(0, 1, 0));
    const lightColor = glm.vec3.fromValues(1, 1, 1);
    const lightPos = glm.vec3.fromValues(0, 10, 0);
    shaders[0].setUniform({ projectionMatrix, viewMatrix, lightColor, lightPos });

    staticMesh.draw(shaders[0], gl.TRIANGLES);
    ballMesh.draw(shaders[0], gl.TRIANGLES);
    dynamicMesh.draw(shaders[0], gl.TRIANGLES);
    dynamicMesh.draw(shaders[0], gl.LINE_LOOP);

    // draw 2d
    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    Polygon2D.getAll().forEach((polygon, i) => polygon.draw());
}


const animate = () => {
    update(1/60);
    draw();
    requestAnimationFrame(animate);
}


const setupDOM = (gameAreaEl: HTMLDivElement, gl: WebGLRenderingContext, ctx: CanvasRenderingContext2D) => {
    ctx.canvas.width = gl.canvas.width = parseFloat(getComputedStyle(gameAreaEl).getPropertyValue("width"));
    ctx.canvas.height = gl.canvas.height = parseFloat(getComputedStyle(gameAreaEl).getPropertyValue("height"));
}



/**
 * Retrive a content from a path as a text
 * @param path the path to get the content from
 * @returns {Promise<unknown>}
 */
const fetchTextFile = path => new Promise((resolve, reject) => {
    fetch(path).then(e => resolve(e.text())).catch(e => reject(e))
});


/**
 * This is a quick function to transform vertex data. especially before uplaoding them to the buffer
 * @param v is the vertices to be transformed
 * @param translate is an array represeting the vec3 translate value
 * @param scale is an array representing the vec3 scale value
 */
const transformVertices = (v: number[], translate: number[], scale: number[]) => {
    let sv = []
    for(let i = 0; i < v.length; i += 3) {
        let x = v[i] * scale[0] + translate[0];
        let y = v[i + 1] * scale[1] + translate[1];
        let z = v[i + 2] * scale[2] + translate[2];
        sv.push(x, y, z);
    }
    return sv;
}


/**
 * This function repeats an array continuosly. It uses the length of the input array as the stride
 * @param arrInp is the array to be repeated
 * @param l is the maximum length of the array
 */
const repeatArray = (arrInp: number[], l: number) => {
    let arr = [];
    for(let i = 0; i < l; i += arrInp.length) arr.push(...arrInp);
    return arr;
}


/**
 * Jquery-like syntax to retrieve a dom element by a css selector
 * @param s is the css selector
 */
const $ = (s: string) => document.querySelector(s);


const eventHandler = () => {

    addEventListener("mousedown", e => {
        evt.pointerHeld = true;
    });

    addEventListener("mouseup", e => {
        evt.pointerHeld = false;
    });

}


/**
 * This is the entry point for the program. this function is called after window.onload event
 */
const main = async() => {
    const gameAreaEl = $("#gameArea") as HTMLDivElement;
    gl = (<HTMLCanvasElement>$("#cvs")).getContext("webgl");
    ctx = (<HTMLCanvasElement>$("#cvs2")).getContext("2d");
    setupDOM(gameAreaEl, gl, ctx);
    shaders.push(new Shader(gl, $("#vertexShader1").textContent, $("#fragmentShader1").textContent));
    await GameManager.init();
    GameManager.startNewLevel();
    gl.enable(gl.CULL_FACE);
    gl.enable(gl.DEPTH_TEST);
    requestAnimationFrame(animate);
    eventHandler();
}

addEventListener("load", main);