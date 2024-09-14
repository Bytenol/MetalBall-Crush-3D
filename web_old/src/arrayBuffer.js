/**
 * This function should be called at the init time. It is used to allocate memory in space representing the VBO.
 * for dynamic buffer, the initial data can be null and length of the buffer should be provided.
 * also the storage type should be called as gl.DYNAMIC_DRAW / gl.STREAM_DRAW etc
 *
 * @param gl    is the webglRenderingContext
 * @param data  is an array|null representing the initial data of the buffer
 * @param length   [optional] is the length of the buffer. default to 0
 * @param storageType   is how the buffer should be expecting changes in data. default to gl.STATIC_DRAW
 *
 * This function returns a webglBuffer object
 */
const createVBO = (gl, data, length = 0, storageType = null) => {
    const type = storageType || gl.STATIC_DRAW;
    const buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    const arr = data ? new Float32Array(data) : new Float32Array(length);
    gl.bufferData(gl.ARRAY_BUFFER, arr, type);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    return buffer;
};
const createEBO = (gl, data, length = 0, storageType = null) => {
    const type = storageType || gl.STATIC_DRAW;
    const buffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
    const arr = data ? new Uint16Array(data) : new Uint16Array(length);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, arr, type);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
    return buffer;
};
const updateEBO = (gl, vbo, data) => {
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(data), gl.STATIC_DRAW);
};
export { createVBO, createEBO, updateEBO };
//# sourceMappingURL=arrayBuffer.js.map