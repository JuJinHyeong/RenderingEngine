
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

export interface Transform {
    position: Vector3;
    rotation: Vector4;
    scale: Vector3;
}

export interface SceneObject {
    id: number;
    name: string;
    type: number;
    transform: Transform;
    parent?: string;
    children?: SceneObject[];
}

export interface Scene {
    name: string;
    objects: SceneObject[];
}
