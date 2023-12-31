import express = require('express');
import OpenAI from 'openai';
import { Scene, SceneObject } from '../models/type';
import { makeTools, functionMap } from '../services/gptService';
const inspect = require("object-inspect");
const dotenv = require('dotenv');
dotenv.config();

const router = express.Router();

const openai = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });

const SetNameArr = (nameArr: string[], object: SceneObject, parentName: string): string[] => {
    nameArr.push(object.name);
    if (object.children) {
        object.children.forEach((child) => {
            SetNameArr(nameArr, child, object.name + ".");
        });
    }
    return nameArr;
}

router.post('/modify_scene', async (req, res) => {
    const scene: Scene = req.body.scene;
    const content: string = req.body.content;
    const makeableList: string[] = req.body.makeables || [];
    const nameArr: string[] = scene.objects.map((object) => object.name);
    console.log(nameArr, makeableList);
    try {
        const completion = await openai.chat.completions.create({
            messages: [
                { "role": "system", "content": "you are a good scene modifier. Use the provided function to answer questions." },
                { "role": "user", "content": content },
                // "Move the position of nano by 10 in the x direction, rotate 90 degrees from the y-axis, scaling twice as big in the z direction"
            ],
            "model": "gpt-3.5-turbo-1106",
            "tools": makeTools(nameArr, makeableList),
        });
        console.log(inspect(completion.choices[0]));
        // change scene... using tools
        completion.choices[0].message.tool_calls.forEach((tool_call) => {
            const functionName = tool_call.function.name;
            if (functionMap.hasOwnProperty(functionName)) {
                const func = functionMap[functionName];
                const argument = JSON.parse(tool_call.function.arguments);
                func(scene, argument);
            }
        });
        res.send(scene);
    } catch (err) {
        console.log(err);
        res.send(err);
    }
});

router.get('/test', async (req: express.Request, res: express.Response) => {
    try {
        const completion = await openai.chat.completions.create({
            messages: [
                { "role": "system", "content": "you are a good scene modifier. Use the provided function to answer questions." },
                { "role": "user", "content": "Move the position of nano by 10 in the x direction, rotate 90 degrees from the y-axis, scaling twice as big in the z direction" },
            ],
            "model": "gpt-3.5-turbo-1106",
            "tools": makeTools(["nano"], []),
        });
        console.log(JSON.stringify(completion.choices[0]));
        res.send(completion.choices[0]);
    } catch (err) {
        res.send(err);
    }
});

export default router;