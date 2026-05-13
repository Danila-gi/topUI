FROM node:18-alpine AS builder

WORKDIR /app

COPY package*.json ./

RUN if [ ! -f package-lock.json ]; then \
        npm install --package-lock-only; \
    fi

RUN npm install

COPY tsconfig.json ./
COPY webpack.config.js ./

COPY src/ ./src/
COPY index.html ./
COPY styles.css ./

RUN npm run build

FROM nginx:alpine

RUN apk add --no-cache curl

COPY --from=builder /app/dist /usr/share/nginx/html/dist
COPY --from=builder /app/index.html /usr/share/nginx/html/
COPY --from=builder /app/styles.css /usr/share/nginx/html/

COPY nginx.conf /etc/nginx/conf.d/default.conf

RUN echo "healthy" > /usr/share/nginx/html/health

EXPOSE 80

HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD curl -f http://localhost/ || exit 1

CMD ["nginx", "-g", "daemon off;"]