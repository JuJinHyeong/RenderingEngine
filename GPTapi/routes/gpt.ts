import express = require('express');
import OpenAI from 'openai';
const router = express.Router();

const openai = new OpenAI({apiKey: process.env.OPENAI_API_KEY})

router.get('/', async (req: express.Request, res: express.Response) => {
    const completion = await openai.chat.completions.create({
        messages: [
            { "role": "system", "content": "you are very helpful assistant" },
            { "role": "user", "content": "give me capital of korea" }
        ],
        "model": "gpt-3.5-turbo-1106"
    });
    res.send(completion.choices[0]);
});

export default router;