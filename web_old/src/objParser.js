export class ObjParser {
    vertices = [];
    uvs = [];
    normals = [];
    constructor(objData) {
        this.#initAttributes(objData);
    }
    #addVertexData(splitted, container) {
        for (let i = 1; i < splitted.length; i++)
            container.push(parseFloat(splitted[i]));
    }
    #initAttributes(data) {
        const _vertices = [];
        const _normals = [];
        const _uvs = [];
        data.split("\n").forEach((line, i) => {
            const str = line.split(" ");
            switch (str[0]) {
                case "vt":
                    this.#addVertexData(str, _uvs);
                    break;
                case "vn":
                    this.#addVertexData(str, _normals);
                    break;
                case "v":
                    this.#addVertexData(str, _vertices);
                    break;
                case "f":
                    for (let i = 1; i < str.length; i++) {
                        let hasDouble = str[i].split("//");
                        let hasOne = str[i].split("/");
                        if (hasDouble.length > 1) {
                            let v = (parseInt(hasDouble[0]) - 1) * 3;
                            let vn = (parseInt(hasDouble[1]) - 1) * 3;
                            for (let i = v; i < v + 3; i++)
                                this.vertices.push(_vertices[i]);
                            for (let i = vn; i < vn + 3; i++)
                                this.normals.push(_normals[i]);
                        }
                        else if (hasOne.length > 1) {
                            let v = (parseInt(hasOne[0]) - 1) * 3;
                            let vt = (parseInt(hasOne[1]) - 1) * 2;
                            for (let i = v; i < v + 3; i++) this.vertices.push(_vertices[i]);
                            for (let i = vt; i < vt + 2; i++) this.uvs.push(_uvs[i]);
                            if (hasOne.length > 2) {
                                let vn = (parseInt(hasOne[2]) - 1) * 3;
                                for (let i = vn; i < vn + 3; i++)
                                    this.normals.push(_normals[i]);
                            }
                        }
                    }
                    break;
            }
        });
    }
}
//# sourceMappingURL=objParser.js.map