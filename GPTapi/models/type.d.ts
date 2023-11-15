
export interface SceneObject {
    id: number;
    name: string;
}

export interface Scene {
    name: string;
    objects: SceneObject[];
}