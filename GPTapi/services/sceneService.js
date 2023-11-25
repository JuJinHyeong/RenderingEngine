"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.SceneModifier = void 0;
const translate = (src, delta) => {
    src.x += delta.x;
    src.y += delta.y;
    src.z += delta.z;
    return src;
};
const rotate = (src, delta) => {
    // rotate
    src.x *= delta.x;
    src.y *= delta.y;
    src.z *= delta.z;
    src.w *= delta.w;
    normalizeVector4(src);
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
exports.SceneModifier = {
    transform,
    Generate
};
//# sourceMappingURL=sceneService.js.map