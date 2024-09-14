export class Texture {

    #gl;
    constructor(gl, src, srcFormat, dstFormat, format, useMipMap = false) {
        this.unit = Texture.unit++;
        this.#gl = gl;
        this.texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, this.texture);
        gl.texImage2D(gl.TEXTURE_2D, 0, srcFormat, dstFormat, format, src);
        if(useMipMap) gl.generateMipmap(gl.TEXTURE_2D);
        gl.bindTexture(gl.TEXTURE_2D, null);
    }

    bind() {
        this.#gl.bindTexture(this.#gl.TEXTURE_2D, this.texture);
    }

    unbind() {
        this.#gl.bindTexture(this.#gl.TEXTURE_2D, null);
    }

    setFilter(min, mag) {
        this.#gl.bindTexture(this.#gl.TEXTURE_2D, this.texture);
        this.#gl.texParameteri(this.#gl.TEXTURE_2D, this.#gl.TEXTURE_MIN_FILTER, min);
        this.#gl.texParameteri(this.#gl.TEXTURE_2D, this.#gl.TEXTURE_MAG_FILTER, mag);
        this.#gl.bindTexture(this.#gl.TEXTURE_2D, null);
    }

    setWrap(s, t) {
        this.#gl.bindTexture(this.#gl.TEXTURE_2D, this.texture);
        this.#gl.texParameteri(this.#gl.TEXTURE_2D, this.#gl.TEXTURE_WRAP_S, s);
        this.#gl.texParameteri(this.#gl.TEXTURE_2D, this.#gl.TEXTURE_WRAP_T, t);
        this.#gl.bindTexture(this.#gl.TEXTURE_2D, null);
    }

    static unit = 0;

}