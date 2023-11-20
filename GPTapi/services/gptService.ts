import OpenAI from 'openai';
import { SceneModifier } from './sceneService';

export const functionMap = {
    transform: SceneModifier.transform,
};

export const makeTools = (objectList: string[]): OpenAI.Chat.Completions.ChatCompletionTool[] => {
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
                                a: {
                                    type: "number",
                                    description: "Value that quaternion first value. real value."
                                },
                                b: {
                                    type: "number",
                                    description: "Value that quaternion second value. i value."
                                },
                                c: {
                                    type: "number",
                                    description: "Value that quaternion third value, j value."
                                },
                                d: {
                                    type: "number",
                                    description: "Value that quaternion last value, k value."
                                }
                            },
                            required: ["a", "b", "c", "d"]
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
}