"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.makeTools = exports.functionMap = void 0;
const sceneService_1 = require("./sceneService");
exports.functionMap = {
    transform: sceneService_1.SceneModifier.transform
};
const makeTools = (objectList) => {
    return [
        {
            type: "function",
            function: {
                name: "transform",
                description: "given name object moves the according to the given translate values, rotates according to the given quantion values, and changes the size according to the given scale values.",
                parameters: {
                    type: "object",
                    properties: {
                        name: {
                            type: "string",
                            description: "Name of the object to transform",
                            enum: objectList
                        },
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
                                x: {
                                    type: "number",
                                    description: "When rotation is expressed as a quaternion, it is the real component."
                                },
                                y: {
                                    type: "number",
                                    description: "When rotation is expressed as a quaternion, it is the i component."
                                },
                                z: {
                                    type: "number",
                                    description: "When rotation is expressed as a quaternion, it is the j component."
                                },
                                w: {
                                    type: "number",
                                    description: "When rotation is expressed as a quaternion, it is the k component."
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
                    },
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
                    properties: {
                        name: {
                            type: "string",
                            description: "Name of the object to generate",
                            enum: objectList
                        },
                        position: {
                            type: "object",
                            properties: {
                                x: {
                                    type: "number",
                                    description: "x value of the location you want to create"
                                },
                                y: {
                                    type: "number",
                                    description: "y value of the location you want to create"
                                },
                                z: {
                                    type: "number",
                                    description: "z value of the location you want to create"
                                }
                            },
                            required: ["x", "y", "z"]
                        },
                    },
                    required: ["name", "position"]
                }
            }
        }
    ];
};
exports.makeTools = makeTools;
//# sourceMappingURL=gptService.js.map