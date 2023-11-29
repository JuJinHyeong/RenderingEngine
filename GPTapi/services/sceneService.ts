import { Scene, Vector3, Vector4 } from "../models/type";
import { quat } from 'gl-matrix';
interface TransformArgument {
    name: string;
    translate?: Vector3;
    quaternion?: Vector4;
    scale?: Vector3;
}
interface GenerateArgument extends TransformArgument {
    parent: string;
}
const translate = (src: Vector3, delta: Vector3) => {
    src.x += delta.x;
    src.y += delta.y;
    src.z += delta.z;
    return src;
}
const rotate = (src: Vector4, delta: Vector4) => {
    // rotate
    let quat1 = quat.create();
    let quat2 = quat.create();
    quat.set(quat1, src.x, src.y, src.z, src.w);
    quat.set(quat2, delta.x, delta.y, delta.z, delta.w);
    quat.multiply(quat1, quat1, quat2);
    src.x = quat1[0];
    src.y = quat1[1];
    src.z = quat1[2];
    src.w = quat1[3];

    return src;
}
const scale = (src: Vector3, delta: Vector3) => {
    src.x *= delta.x;
    src.y *= delta.y;
    src.z *= delta.z;
    return src;
}
const normalizeVector3 = (vec: Vector3) => {
    const sqrtSum = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    vec.x /= sqrtSum;
    vec.y /= sqrtSum;
    vec.z /= sqrtSum;
    return vec;
}
const normalizeVector4 = (vec: Vector4) => {
    const sqrtSum = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
    vec.x /= sqrtSum;
    vec.y /= sqrtSum;
    vec.z /= sqrtSum;
    vec.w /= sqrtSum;
    return vec;
}
const transform = (scene: Scene, { name, translate: deltaPos, quaternion: deltaQuat, scale: deltaScale }: TransformArgument) => {
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
}
const Generate = (scene: Scene, { name, parent, translate: pos, quaternion: rot, scale }: GenerateArgument) => {
    scene.objects.push({
        id: 0,
        name,
        type: 0,
        parent,
        transform: {
            position: pos || { x: 0, y: 0, z: 0 },
            rotation: rot == undefined ? { x: 0, y: 0, z: 0, w: 1 } : normalizeVector4(rot),
            scale: scale || {x: 1, y: 1, z: 1}
        }
    });
}
const ChangeMaterial = (scene: Scene, { name, materialName }: { name: string, materialName: string }) => {
    let obj = scene.objects.find((object) => object.name === name);
    if (obj?.mesh?.material?.name) {
        obj.mesh.material.name = materialName;
    }
}

export const SceneModifier = {
    transform,
    Generate,
    ChangeMaterial,
}