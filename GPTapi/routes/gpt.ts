import express = require('express');
import OpenAI from 'openai';
import { Scene } from '../models/type';
import { makeTools } from '../services/gptService';
const dotenv = require('dotenv');
dotenv.config();

const router = express.Router();

const openai = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });

router.post('/modify_scene', async (req, res) => {
    const scene: Scene = req.body.scene;
    const content: string = req.body.content;
    const nameArr = scene.objects.map((object) => object.name);
    try {
        const completion = await openai.chat.completions.create({
            messages: [
                { "role": "system", "content": "you are a good scene modifier. Use the provided function to answer questions." },
                { "role": "user", "content": content },
                // "Move the position of nano by 10 in the x direction, rotate 90 degrees from the y-axis, scaling twice as big in the z direction"
            ],
            "model": "gpt-3.5-turbo-1106",
            "tools": makeTools(nameArr),
        });
        // change scene... using tools
        console.log(JSON.stringify(completion.choices[0]));
        res.send(completion.choices[0]);
    } catch (err) {
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
            "tools": makeTools([]),
        });
        console.log(JSON.stringify(completion.choices[0]));
        res.send(completion.choices[0]);
    } catch (err) {
        res.send(err);
    }
});

export default router;