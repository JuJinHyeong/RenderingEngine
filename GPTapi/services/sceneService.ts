import { Scene, Vector3, Vector4 } from "../models/type";

type TransformArgument = {
    name: string;
    translate?: Vector3;
    quaternion?: Vector4;
    scale?: Vector3;
}
const translate = (src: Vector3, delta: Vector3) => {
    src.x += delta.x;
    src.y += delta.y;
    src.z += delta.z;
    return src;
}
const rotate = (src: Vector4, delta: Vector4) => {
    // rotate
    src = delta;
    return src;
}
const scale = (src: Vector3, delta: Vector3) => {
    src.x *= delta.x;
    src.y *= delta.y;
    src.z *= delta.z;
    return src;
}
const transform = (scene: Scene, { name, translate: deltaPos, quaternion: deltaQuat, scale: deltaScale }: TransformArgument) => {
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
}
export const SceneModifier = {
    transform
}