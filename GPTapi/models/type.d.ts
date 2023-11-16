
export interface Vector3 {
    x: number;
    y: number;
    z: number;
}

export interface Vector4 {
    x: number;
    y: number;
    z: number;
    w: number;
}

export interface SceneObject {
    id: number;
    name: string;
    position: Vector3;
    quaternion: Vector4;
    scale: Vector3;
}

export interface Scene {
    name: string;
    objects: SceneObject[];
}
