require('dotenv').config({ path: require('path').join(__dirname, '../.env') });
const express = require('express');
const cors = require('cors');

const contactRoute = require('./routes/contact');
const healthRoute = require('./routes/health');

const app = express();
const PORT = process.env.PORT || 3001;

app.use(cors());
app.use(express.json());

app.use('/health', healthRoute);
app.use('/contact', contactRoute);

app.listen(PORT, () => {
  console.log(`xa-notify işləyir: http://localhost:${PORT}`);
});
