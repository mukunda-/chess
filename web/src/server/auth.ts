// NextAuth.js
import DiscordProvider from "next-auth/providers/discord";
import type { DefaultSession, NextAuthOptions } from "next-auth";

import { db } from "@/db/db";

// Auth.js
import { DrizzleAdapter } from "@auth/drizzle-adapter";

// NextAuth.js
import type { Adapter } from "next-auth/adapters";

function getEnv(key: string, required: boolean): string {
  let value = process.env[key];
  if (!value && required) {
    throw new Error(`Missing environment variable: ${key}`);
  } else {
    value ??= "";
  }

  return value;
}

declare module "next-auth" {
  interface Session extends DefaultSession {
    user: {
      id: string;
    } & DefaultSession["user"];
  }
}

export const authOptions: NextAuthOptions = {
  adapter: DrizzleAdapter(db) as Adapter,
  providers: [
    DiscordProvider({
      clientId: getEnv("DISCORD_CLIENT_ID", false),
      clientSecret: getEnv("DISCORD_CLIENT_SECRET", false),
    }),
  ],
  callbacks: {
    session: async ({ session, user }) => {
      return {
        ...session,
        user: {
          ...session.user,
          id: user.id,
        },
      };
    },
  },
};
