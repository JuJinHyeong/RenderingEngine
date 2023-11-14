import express = require('express');
import OpenAI from 'openai';
import { stringify } from 'querystring';
import { tools } from '../services/gptService';
const router = express.Router();

const openai = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });

router.post('/modify_scene', (req, res) => {
    const scene = req.body;
    //scene.width = 1980;
    console.log(scene);
    res.json(scene);
});

router.get('/test', async (req: express.Request, res: express.Response) => {
    try {
        const completion = await openai.chat.completions.create({
            messages: [
                { "role": "system", "content": "you are a good scene modifier. Use the provided function to answer questions." },
                { "role": "user", "content": "Move the position of nano by 10 in the x direction, rotate 90 degrees from the y-axis, scaling twice as big in the z direction" },
            ],
            "model": "gpt-3.5-turbo-1106",
            "tools": tools,
        });
        console.log(JSON.stringify(completion.choices[0]));
        res.send(completion.choices[0]);
    } catch (err) {
        res.send(err);
    }
});

export default router;