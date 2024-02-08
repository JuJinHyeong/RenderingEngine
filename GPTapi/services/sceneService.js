"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.SceneModifier = void 0;
const gl_matrix_1 = require("gl-matrix");
const translate = (src, delta) => {
    src.x += delta.x;
    src.y += delta.y;
    src.z += delta.z;
    return src;
};
const rotate = (src, delta) => {
    // rotate
    let quat1 = gl_matrix_1.quat.create();
    let quat2 = gl_matrix_1.quat.create();
    gl_matrix_1.quat.set(quat1, src.x, src.y, src.z, src.w);
    gl_matrix_1.quat.set(quat2, delta.x, delta.y, delta.z, delta.w);
    gl_matrix_1.quat.multiply(quat1, quat1, quat2);
    src.x = quat1[0];
    src.y = quat1[1];
    src.z = quat1[2];
    src.w = quat1[3];
    return src;
};
const scale = (src, delta) => {
    src.x *= delta.x;
    src.y *= delta.y;
    src.z *= delta.z;
    return src;
};
const normalizeVector3 = (vec) => {
    const sqrtSum = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    vec.x /= sqrtSum;
    vec.y /= sqrtSum;
    vec.z /= sqrtSum;
    return vec;
};
const normalizeVector4 = (vec) => {
    const sqrtSum = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
    vec.x /= sqrtSum;
    vec.y /= sqrtSum;
    vec.z /= sqrtSum;
    vec.w /= sqrtSum;
    return vec;
};
const transform = (scene, { name, translate: deltaPos, quaternion: deltaQuat, scale: deltaScale }) => {
    let obj = scene.objects.find((object) => object.name === name);
    console.log('translate:', deltaPos);
    console.log('rotate:', deltaQuat);
    console.log('scale:', deltaScale);
    if (deltaPos != undefined) {
        obj.transform.position = translate(obj.transform.position, deltaPos);
    }
    if (deltaQuat != undefined) {
        obj.transform.rotation = rotate(obj.transform.rotation, normalizeVector4(deltaQuat));
    }
    if (deltaScale != undefined) {
        obj.transform.scale = scale(obj.transform.scale, deltaScale);
    }
};
const Generate = (scene, { name, parent, translate: pos, quaternion: rot, scale }) => {
    scene.objects.push({
        id: 0,
        name,
        type: 0,
        parent,
        transform: {
            position: pos || { x: 0, y: 0, z: 0 },
            rotation: rot == undefined ? { x: 0, y: 0, z: 0, w: 1 } : normalizeVector4(rot),
            scale: scale || { x: 1, y: 1, z: 1 }
        }
    });
};
const ChangeMaterial = (scene, { name, materialName }) => {
    var _a, _b;
    let obj = scene.objects.find((object) => object.name === name);
    if ((_b = (_a = obj === null || obj === void 0 ? void 0 : obj.mesh) === null || _a === void 0 ? void 0 : _a.material) === null || _b === void 0 ? void 0 : _b.name) {
        obj.mesh.material.name = materialName;
    }
};
const ChangeName = (scene, { name, newName }) => {
    let obj = scene.objects.find((object) => object.name === name);
    if (obj) {
        obj.changedName = newName;
    }
};
exports.SceneModifier = {
    transform,
    Generate,
    ChangeMaterial,
    ChangeName,
};
//# sourceMappingURL=sceneService.js.map