const nodemailer = require('nodemailer');

const transporter = nodemailer.createTransport({
  host: process.env.SMTP_HOST,
  port: process.env.SMTP_PORT,
  secure: false,
  auth: {
    user: process.env.SMTP_USER,
    pass: process.env.SMTP_PASS,
  },
});

async function sendContactEmail({ name, email, phone, message }) {
  await transporter.sendMail({
    from: `"${process.env.FROM_NAME}" <${process.env.FROM_EMAIL}>`,
    to: process.env.FROM_EMAIL,
    subject: `Yeni müraciət — ${name}`,
    html: `
      <h2>Yeni Müraciət</h2>
      <p><strong>Ad:</strong> ${name}</p>
      <p><strong>Email:</strong> ${email}</p>
      <p><strong>Telefon:</strong> ${phone || 'Qeyd edilməyib'}</p>
      <p><strong>Mesaj:</strong></p>
      <p>${message}</p>
    `,
  });
}

async function sendConfirmationEmail({ name, email }) {
  await transporter.sendMail({
    from: `"${process.env.FROM_NAME}" <${process.env.FROM_EMAIL}>`,
    to: email,
    subject: 'Müraciətiniz qəbul edildi — Xa',
    html: `
      <h2>Salam, ${name}!</h2>
      <p>Müraciətiniz uğurla qəbul edildi. Ən qısa zamanda sizinlə əlaqə saxlayacağıq.</p>
      <br/>
      <p>Hörmətlə,<br/><strong>Xa Agency</strong></p>
    `,
  });
}

module.exports = { sendContactEmail, sendConfirmationEmail };
