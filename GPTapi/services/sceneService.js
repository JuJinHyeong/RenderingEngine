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
    src = delta;
    return src;
};
const scale = (src, delta) => {
    src.x *= delta.x;
    src.y *= delta.y;
    src.z *= delta.z;
    return src;
};
const transform = (scene, { name, translate: deltaPos, quaternion: deltaQuat, scale: deltaScale }) => {
    let obj = scene.objects.find((object) => object.name === name);
    console.log('translate:', deltaPos);
    console.log('rotate:', deltaQuat);
    console.log('scale:', deltaScale);
    if (deltaPos != undefined) {
        obj.position = translate(obj.position, deltaPos);
    }
    if (deltaQuat != undefined) {
        obj.quaternion = rotate(obj.quaternion, deltaQuat);
    }
    if (deltaScale != undefined) {
        obj.scale = scale(obj.scale, deltaScale);
    }
};
exports.SceneModifier = {
    transform
};
//# sourceMappingURL=sceneService.js.map