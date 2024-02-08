"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.makeTools = exports.functionMap = void 0;
const sceneService_1 = require("./sceneService");
exports.functionMap = {
    transform: sceneService_1.SceneModifier.transform,
    generate: sceneService_1.SceneModifier.Generate,
    change_material: sceneService_1.SceneModifier.ChangeMaterial,
    change_name: sceneService_1.SceneModifier.ChangeName,
};
const transform = {
    translate: {
        type: "object",
        properties: {
            x: {
                type: "number",
                description: "Value that should be moved in the x-direction"
            },
            y: {
                type: "number",
                description: "Value that should be moved in the y-direction"
            },
            z: {
                type: "number",
                description: "Value that should be moved in the z-direction"
            }
        },
        required: ["x", "y", "z"]
    },
    quaternion: {
        type: "object",
        properties: {
            w: {
                type: "number",
                description: "Value that quaternion first value. real value."
            },
            x: {
                type: "number",
                description: "Value that quaternion second value. i value."
            },
            y: {
                type: "number",
                description: "Value that quaternion third value, j value."
            },
            z: {
                type: "number",
                description: "Value that quaternion last value, k value."
            }
        },
        required: ["x", "y", "z", "w"]
    },
    scale: {
        type: "object",
        properties: {
            x: {
                type: "number",
                description: "Value that should be scaling in the x-direction"
            },
            y: {
                type: "number",
                description: "Value that should be scaling in the y-direction"
            },
            z: {
                type: "number",
                description: "Value that should be scaling in the z-direction"
            }
        },
        required: ["x", "y", "z"]
    }
};
const makeTools = (objectList, makeableList) => {
    return [
        {
            type: "function",
            function: {
                name: "transform",
                description: "given name object moves the according to the given translate values, rotates according to the given quantion values, and changes the size according to the given scale values.",
                parameters: {
                    type: "object",
                    properties: Object.assign({ name: {
                            type: "string",
                            description: "Name of the object to transform",
                            enum: objectList
                        } }, transform),
                    required: ["name"]
                }
            }
        },
        {
            type: "function",
            function: {
                name: "generate",
                description: "generate an object with a given name at the position position.",
                parameters: {
                    type: "object",
                    properties: Object.assign({ name: {
                            type: "string",
                            description: "Name of the object to generate",
                            enum: makeableList
                        }, parent: {
                            type: "string",
                            description: "Name of the parent object to generate",
                            enum: objectList
                        } }, transform),
                    required: ["name"]
                }
            }
        },
        {
            type: "function",
            function: {
                name: "change_material",
                description: "change material given name's object",
                parameters: {
                    type: "object",
                    properties: {
                        name: {
                            type: "string",
                            description: "Name of the object to change material",
                            enum: objectList
                        },
                        materialName: {
                            type: "string",
                            description: "Name of the material to change to",
                            enum: ["brick", "grass", "stone"]
                        }
                    },
                }
            }
        },
        {
            type: "function",
            function: {
                name: "change_name",
                description: "change name given name's object",
                parameters: {
                    type: "object",
                    properties: {
                        name: {
                            type: "string",
                            description: "Name of the object to change material",
                            enum: objectList
                        },
                        newName: {
                            type: "string",
                            description: "Name of the material to change to"
                        }
                    }
                }
            }
        }
    ];
};
exports.makeTools = makeTools;
//# sourceMappingURL=gptService.js.map