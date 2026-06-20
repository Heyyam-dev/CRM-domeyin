const express = require('express');
const router = express.Router();
const { sendContactEmail, sendConfirmationEmail } = require('../services/emailService');

router.post('/', async (req, res) => {
  const { name, email, phone, message } = req.body;

  if (!name || !email || !message) {
    return res.status(400).json({ error: 'Ad, email və mesaj mütləqdir.' });
  }

  try {
    await sendContactEmail({ name, email, phone, message });
    await sendConfirmationEmail({ name, email });
    res.json({ success: true, message: 'Müraciət göndərildi.' });
  } catch (err) {
    console.error('Email xətası:', err.message);
    res.status(500).json({ error: 'Email göndərilmədi.' });
  }
});

module.exports = router;
