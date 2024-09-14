import {ObjParser} from "./objParser.js";
import {createVBO} from "./arrayBuffer.js";
import * as glm from "./esm/index.js";
import {Shader} from "./shader.js";
// export * as glm from "./esm/index.js";


export class Mesh {
    #gl = null;
    vbo = WebGLBuffer;
    translation = glm.vec3.create();
    rotation = 0;

    modelMatrix;
    normalMatrix;
    constructor(gl, vertices = [], uvs = [], normals = [], length = 0, isStatic = true) {
        this.vertices = vertices;
        this.uvs = uvs;
        this.normals = normals;
        this.#gl = gl;
        this.vbo = isStatic ? createVBO(gl, this.data) : createVBO(gl, null, length, gl.DYNAMIC_DRAW);
        if(!isStatic) {
            this.vertices = [];
            this.uvs = [];
            this.normals = [];
        }
    }

    get data() { return [...this.vertices, ...this.uvs, ...this.normals]; }

    get length() { return this.vertices.length; }

    update(dt) {
        this.modelMatrix = glm.mat4.translate(glm.mat4.create(), glm.mat4.create(), this.translation);
        this.modelMatrix = glm.mat4.rotateY(glm.mat4.create(), this.modelMatrix, this.rotation);
        this.normalMatrix = glm.mat4.invert(glm.mat4.create(), this.modelMatrix);
        this.normalMatrix = glm.mat4.transpose(glm.mat4.create(), this.normalMatrix);
    }

    draw(shader, primType) {

        shader.enableAttributes({
            position: { buffer: this.vbo, size: 3 },
            color: { buffer: this.vbo, size: 3, offset: this.length * Float32Array.BYTES_PER_ELEMENT },
            normal: { buffer: this.vbo, size: 3, offset: 2 * this.length * Float32Array.BYTES_PER_ELEMENT }
        });

        shader.setUniform({ modelMatrix: this.modelMatrix, normalMatrix: this.normalMatrix });
        this.#gl.drawArrays(primType, 0, this.length / 3);
    }

}